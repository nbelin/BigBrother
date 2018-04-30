#!/usr/bin/env python
# coding: utf-8

import math
import socket
import sys
import time
import numpy
import pygame
import traceback

def customToRad(angle):
	return math.pi * angle / 2

def degToRad(angle):
	return math.pi * angle / 180

def radToDeg(angle):
	return angle * 180 / math.pi

def anglesToVector(angle_cam, angle_mark):
	# angle_cam is the orientation of the camera
	#  >  0 is to the right
	#  >  1 is to the bottom
	#  > -1 is to the top
	#  >  2 is to the left
	# angle_mark is point of view from Camera.
	# -1 means 90° on its left, 0 is forward, 1 is 90° on its right.
	angle = angle_cam + angle_mark
	rad_angle = customToRad(angle)
	cosa = math.cos(rad_angle)
	sina = math.sin(rad_angle)
	return numpy.array([cosa, -sina])

def areVectorsCollinear(v1, v2):
	# vectors must be unitary, returns true for approx less than 6 degrees
	return abs(v1[0]*v2[1] - v1[1]*v2[0]) < 0.1

def lineFrom2Points(p1, p2):
	A = p1[1] - p2[1]
	B = p2[0] - p1[0]
	C = p2[0]*p1[1] - p1[0]*p2[1]
	return A, B, C

def centroid(robot_poses):
	# warning! take care that the greater the radius, the lower the quality of information
	posSum = 0
	coefSum = 0
	for i in range(len(robot_poses)):
		coef = 1.0/robot_poses[i].radius
		posSum += robot_poses[i].pos * coef
		coefSum += coef
	return map(int, posSum / coefSum)

def distPos(p1, p2):
	dx = p1[0] - p2[0]
	dy = p1[1] - p2[1]
	return math.sqrt(dx*dx + dy*dy)

def isInTable(robot_pos):
	# returns true if the point is in the table.
	# we accept a point slightly out of the table depending the accuracy radius
	minMaxX = 1500 + robot_pos.radius/2
	minY = -robot_pos.radius/2
	maxY = 2000 + robot_pos.radius/2
	return robot_pos.pos[0] > -minMaxX and robot_pos.pos[0] < minMaxX and robot_pos.pos[1] > minY and robot_pos.pos[1] < maxY

def meanOrientations(oris):
	# see "mean of circular quantities" on Google
	# convert all orientations into [cos(a), sin(a)] representation
	poses = []
	for ori in oris:
		if ori >= 0:
			rad_ori = degToRad(ori)
			poses.append([math.cos(rad_ori), math.sin(rad_ori)])
	
	# take the mean of the computed points (will give a point in the unit circle)
	sumPos = [0.0, 0.0]
	nbPos = 0.0
	for pos in poses:
		sumPos[0] += pos[0]
		sumPos[1] += pos[1]
		nbPos += 1
	meanPos = [sumPos[0]/nbPos, sumPos[1]/nbPos]

	# distance between [0, 0] and the meanPos will give us the quality of the information
	dist = math.sqrt(meanPos[0] * meanPos[0] + meanPos[1] * meanPos[1])
	if dist < 0.8:
		return -1

	rad_meanOri = math.atan2(meanPos[1], meanPos[0])
	return int(radToDeg(rad_meanOri) + 360) % 360


def posFrom3Cameras(cams, markers, curTime):
	# today, if one posFrom2Cameras fails, consider we also fail. Could be improved!
	rpos12 = posFrom2Cameras(cams[0:2], markers[0:2], curTime)
	if rpos12.radius < 0:
		return RobotPos()
	rpos23 = posFrom2Cameras(cams[1:3], markers[1:3], curTime)
	if rpos23.radius < 0:
		return RobotPos()
	rpos13 = posFrom2Cameras([cams[0], cams[2]], [markers[0], markers[2]], curTime)
	if rpos13.radius < 0:
		return RobotPos()
	res = centroid([rpos12, rpos23, rpos13])
	# now let's make sure we are not to far from each position
	dist12 = distPos(res, rpos12.pos)
	dist23 = distPos(res, rpos23.pos)
	dist13 = distPos(res, rpos13.pos)
	if dist12 > rpos12.radius + 200 or dist23 > rpos23.radius + 200 or dist13 > rpos13.radius + 200:
		return RobotPos()

	ori = meanOrientations([rpos12.orientation, rpos23.orientation, rpos13.orientation])

	# radxy is at best ~80, and at worst ~550
	# radSum should be between 240 and 1650
	# let's make some more magic to compute the final radius
	# let's take the sum divided by 12 (so between 20 and 137)
	# the distances should be between 40mm and 400mm
	# let's divide them by 4 (to have something between 10mm and 100mm)
	# => final radius should be between 40mm and 300mm
	radSum = rpos12.radius + rpos23.radius + rpos13.radius
	distSum = dist12 + dist23 + dist13
	return RobotPos(res, ori, int( radSum/12 + distSum/4 ))

def posFrom2Cameras(cams, markers, curTime):
	# x = x1 + k1*vx1
	#   = x2 + k2*vx2
	# y = y1 + k1*vy1
	#   = y2 + k2*vy2
	rpos1, vec1 = posFrom1Camera(cams[0], markers[0], curTime)
	rpos2, vec2 = posFrom1Camera(cams[1], markers[1], curTime)
	if distPos(rpos1.pos, rpos2.pos) > rpos1.radius + rpos2.radius + 300:
		# don't bother to go further, if the approximate points viewed by the cameras are to far from each other
		return RobotPos()

	ori = meanOrientations([rpos1.orientation, rpos2.orientation])

	# first, check if vectors are (slightly) collinear because line intersection does not work in this case
	if areVectorsCollinear(vec1, vec2):
		# let's get the centroid of the 2 approx positions, and make sure this is not absurd
		dist12 = distPos(rpos1.pos, rpos2.pos)
		if dist12 > 500:
			# information not good enough
			return RobotPos()
		res = numpy.array(centroid([rpos1, rpos2]))
		# rad1 and rad2 are at best ~100, and at worst ~600
		# dist12/2 is expected to be somewhere between 50mm and 300mm
		# (rad1+rad2)/4 is between 50mm and 500mm
		radius = int( 30 + dist12/2 + (rpos1.radius + rpos2.radius)/4 )
		return RobotPos(res, ori, radius)
	
	# line intersection
	A1, B1, C1 = lineFrom2Points(cams[0].pos, rpos1.pos)
	A2, B2, C2 = lineFrom2Points(cams[1].pos, rpos2.pos)
	D  = A1*B2 - B1*A2
	Dx = C1*B2 - B1*C2
	Dy = A1*C2 - C1*A2
	if D == 0:
		# lines do not intersect, should not happen because collinearity already tested
		return RobotPos()
	res = numpy.array(map(int, [ Dx/float(D), Dy/float(D) ]))
	dist1 = distPos(rpos1.pos, res)
	dist2 = distPos(rpos2.pos, res)
	if dist1 > rpos1.radius + 200 or dist2 > rpos2.radius + 200:
		return RobotPos()

	# rad1 and rad2 are at best ~100, and at worst ~600
	# here some magic, let's take the sum divided by 8 (so between 25 and 150)
	# the sum of distances is expected to be somewhere between 100mm and 1200mm
	# the sum divided by 4 is expected to be between 25mm and 300mm
	# which means 50mm at best, 525mm at worst, => let's add a 30mm flat malus
	radius = int( 30 + (rpos1.radius + rpos2.radius) / 6 + (dist1 + dist2) / 4 )
	return RobotPos(res, ori, radius)

def posFrom1Camera(cam, marker, curTime):
	diffTime = curTime - marker.last_update
	vector = anglesToVector(cam.angle, marker.angle)
	pos = map(int, cam.pos + vector * marker.distance)
	orientation = cam.angle * 90 + marker.orientation
	# the closer the robot to the camera, the more accurate the distance information
	# if the robot detection is recent, the information is more accurate (consider robot's speed is ~200mm/sec)
	# for example:
	#  > 1 second old + 3500 mm from camera => worst case => radius = 30+200+350 = 580 mm
	#  > 0.1 second old + 400 mm from camera => almost best case => radius = 30+20+40 = 90 mm
	#  > 0.5 second old + 500 mm from camera => close but old => radius = 30+100+50 = 180 mm
	#  > 0.2 second old + 2500 mm from camera => far but recent => radius = 30+40+250 = 320 mm
	#  > 0.3 second old + 1500 mm from camera => medium case => radius = 30+60+150 = 240 mm
	radius = int(30 + diffTime * 200 + marker.distance * 0.1)
	return RobotPos(pos, orientation, radius), vector

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
		Gui.start_time = time.time()
		Gui.BLACK = (0, 0, 0)
		Gui.WHITE = (255, 255, 255)
		Gui.RED = (255, 0, 0)
		Gui.GREEN = (0, 255, 0)
		Gui.BLUE = (0, 0, 255)
		Gui.YELLOW = (255, 255, 0)
		pygame.init()
		Gui.font = pygame.font.SysFont("monospace", 25)
		Gui.font_mini = pygame.font.SysFont("monospace", 20)
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
		Gui.timetext = Gui.font.render("Time: " + str(curTime - Gui.start_time), True, Gui.BLACK, Gui.WHITE)
		Gui.window.blit(Gui.timetext, (200, 5))
		for cam in cameras:
			for mark in cam.markers:
				thickness = 2
				difTime = curTime - mark.last_update
				if difTime > 3:
					continue
				elif difTime > 1:
					thickness = 1
				color = Gui.colorFromId(mark.id)
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
			center = Gui.realPos2Gui(rob.rpos.pos)
			rad = Gui.realDist2Gui(rob.rpos.radius)
			color = Gui.colorFromId(rob.id)
			pygame.draw.circle(Gui.window, color, center, rad, 2)
			ori = rob.rpos.orientation
			if ori >= 0:
				vec_line = [rad * math.cos(ori * math.pi / 180.0), rad * math.sin(ori * math.pi / 180.0)]
				end_line = [int(x+y) for x,y in zip(center, vec_line)]
				#print center
				#print end_line
				pygame.draw.line(Gui.window, color, center, end_line, 2)
			text = Gui.font_mini.render("R" + str(rob.id) + ": " + str(rob.rpos.pos), True, [c * 0.5 for c in color], Gui.WHITE)
			Gui.window.blit(text, (150 + 250*int((rob.id -1) % 2), 40 + 20*int((rob.id -1)/2)))
		pygame.display.update()

class RobotPos:
	def __init__(self, _pos=[0, 0], _orientation=-1, _radius=-1):
		self.pos = _pos
		self.orientation = _orientation
		self.radius = _radius

class Robot:
	def __init__(self, Id):
		self.id = Id
		self.idstr = " R" + str(Id) + " "
		self.rpos = RobotPos()
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
			self.rpos, vec = posFrom1Camera(detectedCameras[0], detectedMarkers[0], curTime)
			self.found = self.rpos.radius >= 0 and isInTable(self.rpos)
			return self.found

		if len(detectedCameras) == 2:
			# classic case: simple line intersection
			self.rpos = posFrom2Cameras(detectedCameras, detectedMarkers, curTime)
			self.found = self.rpos.radius >= 0 and isInTable(self.rpos)
			return self.found

		# best scenario, the robot has been detected by the 3 cameras!
		self.rpos = posFrom3Cameras(detectedCameras, detectedMarkers, curTime)
		self.found = self.rpos.radius >= 0 and isInTable(self.rpos)
		return self.found

	def getMessage(self):
		return self.idstr + str(self.rpos.pos[0]) + " " + str(self.rpos.pos[1]) + " " + str(self.rpos.radius)

	def debug(self):
		print ">> I'm robot #" + str(self.id)
		print "pos         = " + str(self.rpos.pos)
		print "orientation = " + str(self.rpos.orientation)
		print "radius      = " + str(self.rpos.radius)
		print "I know " + str(len(self.cameras)) + " cameras"
		print ""

class Marker:
	def __init__(self, Id):
		self.id = Id
		self.angle = 0
		self.distance = 0
		self.orientation = 0
		self.confidence = 0
		self.last_update = 0

	def debug(self):
		print ">> I'm marker #" + str(self.id)
		print "angle       = " + str(self.angle)
		print "distance    = " + str(self.distance)
		print "orientation = " + str(self.orientation)
		print "conf        = " + str(self.confidence)
		print "last_update = " + str(self.last_update)
		print ""

class Camera:
	def __init__(self, Id, pos, angle):
		self.id = Id
		self.pos = numpy.array(map(float, pos))
		self.angle = float(angle)
		self.markers = []
		self.nbMsgRecvd = 0
		self.lastUniqIdRecvd = 0
		for i in range(4):
			self.markers.append(Marker(i+1))

	def update(self, msg, curTime):
		fields = msg.split(" ")
		if self.id != int(fields[0]):
			print "ERROR camera id " + fields[0] + " != " + str(self.id)
			return
		self.nbMsgRecvd += 1
		self.lastUniqIdRecvd = int(fields[1])
		print "[" + str(curTime) + "] camera_id: " + str(self.id) + " (msg " + str(self.nbMsgRecvd) + "/" + str(self.lastUniqIdRecvd) + ") >> " + msg
                marker = []
		for i in range(len(fields) - 2):
			if i % 5 == 0:
				marker = self.markers[int(fields[i + 2]) - 1] # markers[0] has "ID 1"
			elif i % 5 == 1:
				marker.angle = float(fields[i + 2])
			elif i % 5 == 2:
				marker.distance = int(fields[i + 2])
			elif i % 5 == 3:
				marker.orientation = int(fields[i + 2])
			elif i % 5 == 4:
				marker.confidence = float(fields[i + 2])
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
UDP_PORT_START_LISTEN = 0
UDP_ADDR_START_LISTEN = ""
UDP_PORT_START_WRITE = 0
UDP_ADDR_START_WRITE = ""
UDP_PORT_ROBOT_CAST = 0
UDP_ADDR_ROBOT_CAST = ""
UDP_PORT_ROBOT_UNICAST = 0
UDP_ADDR_ROBOT_UNICAST = range(3)
team_side = 0 # 0 for default team (left), 1 for other team (right)
cameras = []
robots = []
last_time_update = 0
wait_game = False
use_gui = False
for arg in sys.argv:
	if arg == "-w":
		wait_game = True
	if arg == "-g":
		use_gui = True


# parse configuration
try:
	config_file = open(config_file_name)
	for line in config_file:
		if line.startswith("#") or line == "\n":
			continue
		line = line.rstrip('\n')
		token, value = line.split("=")
		if token == "UDP_PORT":
			UDP_PORT = int(value)
		elif token == "UDP_PORT_START_LISTEN":
			UDP_PORT_START_LISTEN = int(value)
		elif token == "UDP_ADDR_START_LISTEN":
			UDP_ADDR_START_LISTEN = value
		elif token == "UDP_PORT_START_WRITE":
			UDP_PORT_START_WRITE = int(value)
		elif token == "UDP_ADDR_START_WRITE":
			UDP_ADDR_START_WRITE = value
		elif token == "UDP_PORT_ROBOT_CAST":
			UDP_PORT_ROBOT_CAST = int(value)
		elif token == "UDP_ADDR_ROBOT_CAST":
			UDP_ADDR_ROBOT_CAST = value
		elif token == "UDP_PORT_ROBOT_UNICAST":
			UDP_PORT_ROBOT_UNICAST = int(value)
		elif token.startswith("POSITION_"):
			pos_id = int(token.replace("POSITION_", ""))
			if pos_id < 0 or pos_id > 2:
				raise SyntaxError("Invalid position id: " + token)
			coords = value.split(",")
			if len(coords) != 3:
				raise SyntaxError("Invalid position value: " + value)
			cameras.append(Camera(pos_id, coords[0:2], coords[2]))
		elif token.startswith("UDP_ADDR_ROBOT_UNICAST_"):
			rob_id = int(token.replace("UDP_ADDR_ROBOT_UNICAST_", ""))
			UDP_ADDR_ROBOT_UNICAST[rob_id] = value
		else:
			raise SyntaxError("Unrecognized token:" + token)
	if UDP_PORT == 0:
		raise SyntaxError("UDP_PORT has not been defined")
	if UDP_PORT_ROBOT_CAST == 0:
		raise SyntaxError("UDP_PORT_ROBOT_CAST has not been defined")
	if wait_game and UDP_PORT_START_LISTEN == 0:
		raise SyntaxError("UDP_PORT_START_LISTEN has not been defined (-w has been used)")
	if wait_game and UDP_PORT_START_WRITE == 0:
		raise SyntaxError("UDP_PORT_START_WRITE has not been defined (-w has been used)")
except Exception as e:
	print repr(e)
	sys.exit("Error reading config file " + config_file_name)
config_file.close()

# init robots
for i in range(4):
	robots.append(Robot(i+1))
	for cam in cameras:
		robots[i].addCamera(cam)

# print configuration
for rob in robots:
	rob.debug()
for cam in cameras:
	cam.debug()

if len(cameras) != 3:
	sys.exit("3 cameras must be defined")

# prepare sockets
sock_cameras = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
sock_cameras.bind(("", UDP_PORT))
sock_cameras.settimeout(0)
# (multi cast)
sock_robots = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP) 
sock_robots.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

# real game condition (wait for game to start)
if wait_game:
	# UDP
	sock_start_listen = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock_start_listen.bind((UDP_ADDR_START_LISTEN, UDP_PORT_START_LISTEN))
	sock_start_listen.settimeout(60)

	sock_start_write = []
	if UDP_PORT_ROBOT_UNICAST == 0:
		# UDP MULTICAST
		sock_start_write = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP) 
		sock_start_write.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	else:
		sock_start_write = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

	arg_markers = "1234"

	print "Waiting for game to start... (port " + str(UDP_PORT_START_LISTEN) + ")"
	sys.stdout.flush()
	while False:
		try:
			#arg_markers = "123"  # uncomment to test if no robot is up
			#break
			data = sock_start_listen.recv(1024)
			data_tokens = data.split(" ")
			if data_tokens[0] != "GO":
				raise RuntimeError("Message should start with 'GO' but started with: " + str(data_tokens[0]))
			team_side = int(data_tokens[1])
			arg_markers = data_tokens[2]
			break
		except Exception as e:
			print repr(e)
		sys.stdout.flush()
	
	time.sleep(60)
	print "arg_markers: ", arg_markers
	for i in range(4):
		target_port = 0
		if UDP_PORT_ROBOT_UNICAST == 0: #multicast
			target_addrs = [UDP_ADDR_ROBOT_CAST]
			target_port = UDP_PORT_ROBOT_CAST
		else:
			target_addrs = UDP_ADDR_ROBOT_UNICAST
			target_port = UDP_PORT_ROBOT_UNICAST

		for target_addr in target_addrs:
			try:
				print "send: GO " + arg_markers + " to " + target_addr + ":" + str(target_port) + "..."
				sock_start_write.sendto("GO " + arg_markers, (target_addr, target_port))
			except Exception as e:
				print repr(e)
		sys.stdout.flush()
		time.sleep(0.2)

if team_side == 1:
	# need to change default camera positions
	for cam in cameras:
		cam.pos[0] = -cam.pos[0]
		cam.angle = 1 + (1 - cam.angle)

if use_gui:
	Gui.initGui()


#### data to test perf and corner cases
cameras[0].markers[0].last_update = time.time()
cameras[1].markers[0].last_update = time.time()
cameras[2].markers[0].last_update = time.time()
cameras[0].markers[0].angle = 0.39
cameras[1].markers[0].angle = -0.39
cameras[2].markers[0].angle = 0.01
cameras[0].markers[0].distance = 800
cameras[1].markers[0].distance = 1000
cameras[2].markers[0].distance = 2582
cameras[0].markers[0].orientation = 135
cameras[1].markers[0].orientation = 225
cameras[2].markers[0].orientation = 50
#cameras[0].markers[1].last_update = time.time()
#cameras[1].markers[1].last_update = time.time()
#cameras[2].markers[1].last_update = time.time()
cameras[0].markers[1].angle = 0.22
cameras[1].markers[1].angle = -0.29
cameras[2].markers[1].angle = 0.09
cameras[0].markers[1].distance = 900
cameras[1].markers[1].distance = 1400
cameras[2].markers[1].distance = 2382
#cameras[0].markers[2].last_update = time.time()
#cameras[1].markers[2].last_update = time.time()
#cameras[2].markers[2].last_update = time.time()
cameras[0].markers[2].angle = 0.19
cameras[1].markers[2].angle = -0.04
cameras[2].markers[2].angle = -0.07
cameras[0].markers[2].distance = 1600
cameras[1].markers[2].distance = 1100
cameras[2].markers[2].distance = 1582
#cameras[0].markers[3].last_update = time.time()
#cameras[1].markers[3].last_update = time.time()
#cameras[2].markers[3].last_update = time.time()
cameras[0].markers[3].angle = -0.42
cameras[1].markers[3].angle = 0.04
cameras[2].markers[3].angle = 0.48
cameras[0].markers[3].distance = 2600
cameras[1].markers[3].distance = 3200
cameras[2].markers[3].distance = 1382


print ""
print "Listenning port " + str(UDP_PORT) + "..."
sys.stdout.flush()
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
				print "[" + str(now) + "] SEND: " + msg
				try:
					sock_robots.sendto(msg, (UDP_ADDR_ROBOT_CAST, UDP_PORT_ROBOT_CAST))
				except Exception as e:
					print repr(e)
			if use_gui:
				Gui.updateGui(now)
			last_time_update = now

		# let's see if we received any data from cameras
		for i in range(len(cameras)):
			data = sock_cameras.recv(1024)
			cam_id = int(data[0])
			cameras[cam_id].update(data, now)
		#cameras[cam_id].debug()
		sys.stdout.flush()

		# listen to any client who wants information
	except Exception as e:
		#print repr(e)
		#traceback.print_exc()
		pass
