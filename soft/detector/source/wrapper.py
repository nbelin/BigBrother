#!/usr/bin/env python
# coding: utf-8

import socket
import sys
import time
import subprocess
import os
import struct

####################
####### main #######
####################

# global var config
config_file_name = "../config/config_wrapper.txt"
UDP_PORT = 0
UDP_ADDR = ""
no_wait = False
for arg in sys.argv:
        if arg == "-nowait":
                no_wait = True


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
		elif token == "UDP_ADDR":
			UDP_ADDR = value
		else:
			raise SyntaxError("Unrecognized token:" + token)
	if UDP_PORT == 0:
		raise SyntaxError("UDP_PORT has not been defined")
except Exception as e:
	print repr(e)
	sys.exit("Error reading config file " + config_file_name)
config_file.close()


# prepare socket
sock_triang = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP) # UDP MULTICAST
sock_triang.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock_triang.bind((UDP_ADDR, UDP_PORT))
mreq = struct.pack("4sl", socket.inet_aton(UDP_ADDR), socket.INADDR_ANY)
sock_triang.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)
sock_triang.settimeout(60)

nb_retry = 0
arg_marker = ""

# wait for game to start
print ""
print "Listenning port " + str(UDP_PORT) + "..."
while not no_wait:
	try:
		print "nb_retry: ", nb_retry
		data = sock_triang.recv(1024)
		print "received: ", data
		if data.startswith("GO "):
			arg_marker = data.split(" ")[1]
			break
		print "wrong message!"
	except Exception as e:
		print repr(e)
		pass
	nb_retry += 1

# enable raspicam, let the code continue even if the command fails
try:
	print "run: sudo modprobe bcm2835-v4l2"
	subprocess.call("sudo modprobe bcm2835-v4l2", shell=True)
except Exception as e:
	print repr(e)
# delay a shutdown, let the code continue even if the command fails
try:
	print "run: sleep 110 && sudo shutdown -h now &"
	subprocess.call("sleep 110 && sudo shutdown -h now &", shell=True)
	print "!! The system will shutdown in 110 seconds, kill the sleep process to abort the shutdown !!"
except Exception as e:
	print repr(e)

# prepare directories
output_dir_template = "../results/"
try:
	print "create ", output_dir_template
	os.mkdir(output_dir_template)
except Exception as e:
	print repr(e)

# try in loop to create a new dir for this game
while True:
	output_dir = output_dir_template + "game_" + str(time.time()) + "/"
	try:
		print "create ", output_dir
		os.mkdir(output_dir)
	except Exception as e:
		print repr(e)
		pass
	break

# run main process
command_line_template = "../build/detector"
print "cmd_line_template: ", command_line_template
for i in range(20):
	arg_output = output_dir + "video" + str(i) + ".avi"
	stdout_str = output_dir + "out" + str(i) + ".txt"
	stderr_str = output_dir + "err" + str(i) + ".txt"
	command_line = [command_line_template, "-m=" + arg_marker, "-o=" + arg_output]
	print "run: ", command_line
	print "stdout: ", stdout_str
	print "stderr: ", stderr_str
	ret = 123
        with open(stdout_str, "w") as stdout_file:
        	with open(stderr_str, "w") as stderr_file:
			ret = subprocess.call(command_line, stdout=stdout_file, stderr=stderr_file)
	print "returned: ", ret
	if ret == 0:
		break

print "end"
