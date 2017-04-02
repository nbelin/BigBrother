#!/usr/bin/env python
# coding: utf-8

import socket
import sys
import time

class Robot:
	def __init__(self, id):
		self.id = id
		self.pos = [.0, .0]
		self.cameras = []

	def addCamera(self, camera):
		self.cameras.append(camera)

	def updatePos(self, curTime):
		return ""

	def debug(self):
		print ">> I'm robot #" + str(self.id)
		print "I know " + str(len(self.cameras)) + " cameras"
		print ""

class Marker:
	def __init__(self, id):
		self.id = id
		self.x = 0
		self.height = 0
		self.confidence = 0
		self.last_update = 0

	def debug(self):
		print ">> I'm marker #" + str(self.id)
		print "x      = " + str(self.x)
		print "height = " + str(self.height)
		print "conf   = " + str(self.confidence)
		print "last_u = " + str(self.last_update)
		print ""

class Camera:
	def __init__(self, id, pos):
		self.id = id
		self.pos = map(float, pos)
		self.markers = []
		for i in range(4):
			self.markers.append(Marker(i))

	def update(self, msg):
		fields = msg.split(" ")
		if self.id != int(fields[0]):
			print "ERROR camera id " + fields[0] + " != " + str(self.id)
			return
		print "camera_id: " + str(self.id)
                marker = []
		for i in range(len(fields) - 1):
			if i % 4 == 0:
				marker = self.markers[int(fields[i + 1])]
			elif i % 4 == 1:
				marker.x = float(fields[i + 1])
			elif i % 4 == 2:
				marker.height = float(fields[i + 1])
			elif i % 4 == 3:
				marker.confidence = float(fields[i + 1])
				marker.last_update = time.time()

	def debug(self):
		print ">> I'm camera #" + str(self.id)
		print "pos = " + str(self.pos)
		print "I know " + str(len(self.markers)) + " markers:"
		for mark in self.markers:
			mark.debug()
		print ""

####################
####### main #######
####################

# global var config
config_file_name = "../config/config.txt"
UDP_PORT = 0
POSITIONS = [[],[],[]]
cameras = []
robots = []
last_time_update = 0

# parse configuration
try:
	config_file = open(config_file_name)
	for line in config_file:
		if line.startswith("#") or line == "\n":
			continue
		token, value = line.split("=")
		if token == "UDP_PORT":
			UDP_PORT = int(value)
		elif token.startswith("POSITION_"):
			pos_id = int(token.replace("POSITION_", ""))
			if pos_id < 0 or pos_id > 2:
				raise SyntaxError("Invalid position id: " + token)
			coords = value.split(",")
			if len(coords) != 2:
				raise SyntaxError("Invalid position value: " + value)
			cameras.append(Camera(pos_id, coords))
			POSITIONS[pos_id] = [0,0]
			for i in range(2):
				POSITIONS[pos_id][i] = int(coords[i])
		else:
			raise SyntaxError("Unrecognized token:" + token)
	if UDP_PORT == 0:
		raise SyntaxError("UDP_PORT has not been defined")
	for i in range(3):
		if len(POSITIONS[i]) != 2:
			raise SyntaxError("POSITION_" + str(i) + " has not been defined")
except Exception as e:
	print repr(e)
	sys.exit("Error reading config file " + config_file_name)
config_file.close()

for i in range(3):
	print "Balise " + str(i) + " is at " + str(POSITIONS[i])

# init robots
for i in range(4):
	robots.append(Robot(i))
	for cam in range(3):
		robots[i].addCamera(cameras[cam])


# print configuration
for rob in robots:
	rob.debug()
for cam in cameras:
	cam.debug()

# prepare socket
sock_cameras = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
sock_cameras.bind(("", UDP_PORT))
sock_cameras.settimeout(0.01)

# last info received from cameras

print ""
print "Listenning port " + str(UDP_PORT) + "..."
while True:
	time.sleep(0.01) # avoid to burn 100% CPU
	try:
		# if enough time has elapsed since last update, we can resend our info to the robots
		now = time.time()
		diff = now - last_time_update
		if diff > 0.1:
			print str(now)
			msg = ""
			for rob in robots:
				msg += rob.updatePos(now)
			if len(msg) > 0:
				# send message to robots
				pass
			last_time_update = now

		# let's see if we received any data from cameras
		data, addr = sock_cameras.recvfrom(1024)
		cam_id = int(data[0])
		cameras[cam_id].update(data)
		cameras[cam_id].debug()

		# listen to any client who wants information
	except:
		pass
