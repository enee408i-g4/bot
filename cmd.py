#!/usr/bin/python

import serial
import time
import sys

arduino = None

for i in range(0, 4):
	try:
		arduino = serial.Serial('/dev/ttyACM%d'  % i, 19200)
		break
	except: pass

print sys.argv
arduino.write("{\"command\":\"%s\"}\n" % sys.argv[1]);
