#!/usr/bin/env python
# coding: utf-8

import socket, sys

# global var config
config_file_name = "../config/config.txt"
UDP_PORT = 0
POSITIONS = [[],[],[]]

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

# prepare socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
sock.bind(("", UDP_PORT))

# last info received from cameras

print "Listenning port " + str(UDP_PORT) + "..."
for i in range(5):
	data, addr = sock.recvfrom(1024)
	fields = data.split(" ")
	print fields
	camera_id = int(fields[0])
	print "camera_id: " + str(camera_id)
	for i in range(len(fields) - 1):
		if i % 4 == 0:
			print "  marker_id: " + fields[i + 1]
		elif i % 4 == 1:
			print "    x: " + fields[i + 1]
		elif i % 4 == 2:
			print "    height: " + fields[i + 1]
		elif i % 4 == 3:
			print "    confidence: " + fields[i + 1]
