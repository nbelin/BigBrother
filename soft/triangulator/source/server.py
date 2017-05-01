#!/usr/bin/env python
# coding: utf-8

import math
import socket
import sys
import time
import numpy
import pygame


def anglesToVector(angle_cam, angle_mark):
	# angle_cam is the orientation of the camera
	#  >  0 is to the right
	#  >  1 is to the bottom
	#  > -1 is to the top
	#  >  2 is to the left
	# angle_mark is point of view from Camera.
	# -1 means 90° on its left, 0 is forward, 1 is 90° on its right.
	angle = angle_cam + angle_mark
	rad_angle = math.pi * angle / 2
	cosa = math.cos(rad_angle)
	sina = math.sin(rad_angle)
	return numpy.array([cosa, sina])

def areVectorsCollinear(v1, v2):
	# vectors must be unitary, returns true for approx less than 6 degrees
	return abs(v1[0]*v2[1] - v1[1]*v2[0]) < 0.1

def lineFrom2Points(p1, p2):
	A = p1[1] - p2[1]
	B = p2[0] - p1[0]
	C = p2[0]*p1[1] - p1[0]*p2[1]
	return A, B, C

def centroid(poses, radiuses):
	# warning! take care that the greater the radius, the lower the quality of information
	posSum = 0
	coefSum = 0
	for i in range(len(radiuses)):
		coef = 1.0/radiuses[i]
		posSum += poses[i] * coef
		coefSum += coef
	return map(int, posSum / coefSum)

def distPos(p1, p2):
	dx = p1[0] - p2[0]
	dy = p1[1] - p2[1]
	return math.sqrt(dx*dx + dy*dy)

def isInTable(point, radius):
	# returns true if the point is in the table.
	# we accept a point slightly out of the table depending the accuracy radius
	minMaxX = 1500 + radius/2
	minY = -radius/2
	maxY = 2000 + radius/2
	return point[0] > -minMaxX and point[0] < minMaxX and point[1] > minY and point[1] < maxY

def posFrom3Cameras(cams, markers, curTime):
	# today, if one posFrom2Cameras fails, consider we also fail. Could be improved!
	pos12, rad12 = posFrom2Cameras(cams[0:2], markers[0:2], curTime)
	if rad12 < 0:
		return [], -1
	pos23, rad23 = posFrom2Cameras(cams[1:3], markers[1:3], curTime)
	if rad23 < 0:
		return [], -1
	pos13, rad13 = posFrom2Cameras([cams[0], cams[2]], [markers[0], markers[2]], curTime)
	if rad13 < 0:
		return [], -1
	res = centroid([pos12, pos23, pos13], [rad12, rad23, rad13])
	# now let's make sure we are not to far from each position
	dist12 = distPos(res, pos12)
	dist23 = distPos(res, pos23)
	dist13 = distPos(res, pos13)
	if dist12 > rad12 + 300 or dist23 > rad23 + 300 or dist13 > rad13 + 300:
		return [], -1
	# radxy is at best ~80, and at worst ~550
	# radSum should be between 240 and 1650
	# let's make some more magic to compute the final radius
	# let's take the sum divided by 12 (so between 20 and 137)
	# the distances should be between 40mm and 400mm
	# let's divide them by 4 (to have something between 10mm and 100mm)
	# => final radius should be between 40mm and 300mm
	radSum = rad12 + rad23 + rad13
	distSum = dist12 + dist23 + dist13
	return res, int( radSum/12 + distSum/4 )

def posFrom2Cameras(cams, markers, curTime):
	# x = x1 + k1*vx1
	#   = x2 + k2*vx2
	# y = y1 + k1*vy1
	#   = y2 + k2*vy2
	pos1, rad1, vec1 = posFrom1Camera(cams[0], markers[0], curTime)
	pos2, rad2, vec2 = posFrom1Camera(cams[1], markers[1], curTime)
	if distPos(pos1, pos2) > rad1 + rad2 + 500:
		# don't bother to go further, if the approximate points viewed by the cameras are to far from each other
		return [], -1
	# first, check if vectors are (slightly) collinear because line intersection does not work in this case
	if areVectorsCollinear(vec1, vec2):
		# let's get the centroid of the 2 approx positions, and make sure this is not absurd
		dist12 = distPos(pos1, pos2)
		if dist12 > 800:
			# information not good enough
			return [], -1
		res = numpy.array(centroid([pos1, pos2], [rad1, rad2]))
		# rad1 and rad2 are at best ~100, and at worst ~1000
		# dist12/2 is expected to be somewhere between 50mm and 400mm
		# (rad1+rad2)/4 is between 50mm and 500mm
		radius = int( 30 + dist12/2 + (rad1 + rad2)/4 )
		return res, radius
	
	# line intersection
	A1, B1, C1 = lineFrom2Points(cams[0].pos, pos1)
	A2, B2, C2 = lineFrom2Points(cams[1].pos, pos2)
	D  = A1*B2 - B1*A2
	Dx = C1*B2 - B1*C2
	Dy = A1*C2 - C1*A2
	if D == 0:
		# lines do not intersect, should not happen because collinearity already tested
		return [], -1
	res = numpy.array(map(int, [ Dx/float(D), Dy/float(D) ]))
	dist1 = distPos(pos1, res)
	dist2 = distPos(pos2, res)
	if dist1 > rad1 + 400 or dist2 > rad2 + 400:
		return [], -1

	# rad1 and rad2 are at best ~100, and at worst ~900
	# here some magic, let's take the sum divided by 8 (so between 25 and 225)
	# the sum of distances is expected somewhere to be between 100mm and 1200mm
	# the sum divided by 4 is expected to be between 25mm and 300mm
	# which means 50mm at best, 525mm at worst, => let's add a 30mm flat malus
	radius = int( 30 + (rad1 + rad2) / 6 + (dist1 + dist2) / 4 )
	return res, radius

def posFrom1Camera(cam, marker, curTime):
	diffTime = curTime - marker.last_update
	vector = anglesToVector(cam.angle, marker.angle)
	pos = map(int, cam.pos + vector * marker.distance)
	# the closer the robot to the camera, the more accurate the distance information
	# if the robot detection is recent, the information is more accurate (consider robot's speed is ~200mm/sec)
	# for example:
	#  > 1 second old + 3500 mm from camera => worst case => radius = 200+875 = 1075 mm
	#  > 0.1 second old + 400 mm from camera => almost best case => radius = 20+100 = 120 mm
	#  > 0.5 second old + 500 mm from camera => close but old => radius = 100+125 = 225 mm
	#  > 0.2 second old + 2500 mm from camera => far but recent => radius = 40+625 = 665 mm
	#  > 0.3 second old + 1500 mm from camera => medium case => radius = 60+375 = 435 mm
	radius = int(diffTime * 200 + marker.distance * 0.25)
	return numpy.array(pos), radius, vector

class Gui:
	@staticmethod
	def realPos2Gui(pos):
		# pos is between (-1500, 2000) and (1500, 0)
		# must return something between (50, 80) and (650, 480)
		x = int( ( ( pos[0] + 1500.0 ) / 3000.0 ) * 600.0 + 70)
		y = int( ( ( 2000 - pos[1] ) / 2000.0 ) * 400.0 + 100)
		return [x, y]

	@staticmethod
	def realDist2Gui(dist):
		return int( dist*600.0 / 3000.0 )

	@staticmethod
	def colorFromId(Id):
		if Id == 1:
			return Gui.RED
		if Id == 2:
			return Gui.GREEN
		if Id == 3:
			return Gui.BLUE
		if Id == 4:
			return Gui.YELLOW

	@staticmethod
	def initGui():
		Gui.BLACK = (0, 0, 0)
		Gui.WHITE = (255, 255, 255)
		Gui.RED = (255, 0, 0)
		Gui.GREEN = (0, 255, 0)
		Gui.BLUE = (0, 0, 255)
		Gui.YELLOW = (255, 255, 0)
		pygame.init()
		Gui.font = pygame.font.SysFont("monospace", 25)
		Gui.window = pygame.display.set_mode((740, 570), 0, 32)
		Gui.drawTable()

	@staticmethod
	def drawTable():
		Gui.window.fill(Gui.WHITE)
		pygame.draw.rect(Gui.window, Gui.BLACK, (70, 100, 600, 400), 2)
		for i in range(500, 2000, 500):
			pygame.draw.line(Gui.window, Gui.BLACK, Gui.realPos2Gui([-1500, i]), Gui.realPos2Gui([1500, i]))
		for i in range(-1500, 1500, 500):
			pygame.draw.line(Gui.window, Gui.BLACK, Gui.realPos2Gui([i, 0]), Gui.realPos2Gui([i, 2000]))
		for cam in cameras:
			pygame.draw.circle(Gui.window, Gui.BLACK, Gui.realPos2Gui(cam.pos), 15, 2)
		pygame.display.update()

	@staticmethod
	def updateGui(curTime):
		Gui.drawTable()
		Gui.timetext = Gui.font.render("Time: " + str(curTime), True, Gui.BLACK, Gui.WHITE)
		Gui.window.blit(Gui.timetext, (50, 20))
		for cam in cameras:
			for mark in cam.markers:
				color = Gui.colorFromId(mark.id)
				thickness = 2
				difTime = curTime - mark.last_update
				if difTime > 2:
					continue
				elif difTime > 1:
					thickness = 1
				start_line = Gui.realPos2Gui(cam.pos)
				end_line = Gui.realPos2Gui(cam.pos + 2*mark.distance*anglesToVector(mark.angle, cam.angle))
				approx_point = Gui.realPos2Gui(cam.pos + mark.distance*anglesToVector(mark.angle, cam.angle))
				pygame.draw.line(Gui.window, color, start_line, end_line, thickness)
				pygame.draw.circle(Gui.window, color, approx_point, 3+thickness, 3+thickness)
		for rob in robots:
			if rob.found == False:
				continue
			difTime = curTime - rob.last_update
			if difTime > 2:
				continue
			center = Gui.realPos2Gui(rob.pos)
			rad = Gui.realDist2Gui(rob.radius)
			color = Gui.colorFromId(rob.id)
			pygame.draw.circle(Gui.window, color, center, rad, 2)
		pygame.display.update()

class Robot:
	def __init__(self, Id):
		self.id = Id
		self.idstr = " R" + str(Id) + " "
		self.pos = [0, 0]
		self.radius = 0
		self.cameras = []
		self.last_update = 0
		self.found = False

	def addCamera(self, camera):
		self.cameras.append(camera)

	def updatePos(self, curTime):
		detectedCameras = []
		detectedMarkers = []
		for cam in cameras:
			diffTime = curTime - cam.markers[self.id - 1].last_update
			if diffTime < 2:
				# if diffTime is > 2 seconds, consider information is out of date
				detectedCameras.append(cam)
				detectedMarkers.append(cam.markers[self.id - 1])
		self.last_update = curTime
		if len(detectedCameras) == 0:
			# robot has not been detected by any camera
			self.found = False
			self.radius = 0
			return False

		if len(detectedCameras) == 1:
			# only one camera... let's try anyway to send some information
			self.pos, self.radius, vec = posFrom1Camera(detectedCameras[0], detectedMarkers[0], curTime)
			self.found = self.radius >= 0 and isInTable(self.pos, self.radius)
			return self.found

		if len(detectedCameras) == 2:
			# classic case: simple line intersection
			self.pos, self.radius = posFrom2Cameras(detectedCameras, detectedMarkers, curTime)
			self.found = self.radius >= 0 and isInTable(self.pos, self.radius)
			return self.found

		# best scenario, the robot has been detected by the 3 cameras!
		self.pos, self.radius = posFrom3Cameras(detectedCameras, detectedMarkers, curTime)
		self.found = self.radius >= 0 and isInTable(self.pos, self.radius)
		return self.found

	def getMessage(self):
		return self.idstr + str(self.pos[0]) + " " + str(self.pos[1]) + " " + str(self.radius)

	def debug(self):
		print ">> I'm robot #" + str(self.id)
		print "pos    = " + str(self.pos)
		print "radius = " + str(self.radius)
		print "I know " + str(len(self.cameras)) + " cameras"
		print ""

class Marker:
	def __init__(self, Id):
		self.id = Id
		self.angle = 0
		self.distance = 0
		self.confidence = 0
		self.last_update = 0

	def debug(self):
		print ">> I'm marker #" + str(self.id)
		print "angle    = " + str(self.angle)
		print "distance = " + str(self.distance)
		print "conf     = " + str(self.confidence)
		print "last_upd = " + str(self.last_update)
		print ""

class Camera:
	def __init__(self, Id, pos, angle):
		self.id = Id
		self.pos = numpy.array(map(float, pos))
		self.angle = float(angle)
		self.markers = []
		for i in range(4):
			self.markers.append(Marker(i+1))

	def update(self, msg, curTime):
		fields = msg.split(" ")
		if self.id != int(fields[0]):
			print "ERROR camera id " + fields[0] + " != " + str(self.id)
			return
		print "camera_id: " + str(self.id)
                marker = []
		for i in range(len(fields) - 1):
			if i % 4 == 0:
				marker = self.markers[int(fields[i + 1]) - 1] # markers[0] has "ID 1"
			elif i % 4 == 1:
				marker.angle = float(fields[i + 1])
			elif i % 4 == 2:
				marker.distance = int(fields[i + 1])
			elif i % 4 == 3:
				marker.confidence = float(fields[i + 1])
				marker.last_update = curTime

	def debug(self):
		print ">> I'm camera #" + str(self.id)
		print "pos   = " + str(self.pos)
		print "angle = " + str(self.angle)
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
cameras = []
robots = []
last_time_update = 0
use_gui = False
for arg in sys.argv:
	if arg == "-g":
		use_gui = True


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
			if len(coords) != 3:
				raise SyntaxError("Invalid position value: " + value)
			cameras.append(Camera(pos_id, coords[0:2], coords[2]))
		else:
			raise SyntaxError("Unrecognized token:" + token)
	if UDP_PORT == 0:
		raise SyntaxError("UDP_PORT has not been defined")
except Exception as e:
	print repr(e)
	sys.exit("Error reading config file " + config_file_name)
config_file.close()

# init robots
for i in range(4):
	robots.append(Robot(i+1))
	for cam in range(3):
		robots[i].addCamera(cameras[cam])

# print configuration
for rob in robots:
	rob.debug()
for cam in cameras:
	cam.debug()

if len(cameras) != 3:
	sys.exit("3 cameras must be defined")

# prepare socket
sock_cameras = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
sock_cameras.bind(("", UDP_PORT))
sock_cameras.settimeout(0)

# last info received from cameras


if use_gui:
	Gui.initGui()

print ""
print "Listenning port " + str(UDP_PORT) + "..."
while True:
	time.sleep(0.05) # avoid to burn 100% CPU... and give a chance to a CtrlC :)
	try:
		# if enough time has elapsed since last update, we can resend our info to the robots
		now = time.time()
		diff = now - last_time_update
		if diff > 0.15:
			#print str(now)
			msg = ""
			for rob in robots:
				if rob.updatePos(now):
					msg += rob.getMessage()
			if len(msg) > 0:
				# send message to robots
				print "SEND: " + msg
			if use_gui:
				Gui.updateGui(now)
			last_time_update = now

		# let's see if we received any data from cameras
		for i in range(len(cameras)):
			data, addr = sock_cameras.recvfrom(1024)
			cam_id = int(data[0])
			cameras[cam_id].update(data, now)
		#cameras[cam_id].debug()

		# listen to any client who wants information
	except Exception as e:
		#print repr(e)
		pass
