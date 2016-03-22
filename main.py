#!/usr/bin/python

import json
import serial
import sys
sys.path.append('/home/pi/pixy/build/libpixyusb_swig')
from pixy import *
from ctypes import *

PIXY_MIN_X             =    0
PIXY_MAX_X             =  319
PIXY_MIN_Y             =    0
PIXY_MAX_Y             =  199
PIXY_X_CENTER          =  ((PIXY_MAX_X-PIXY_MIN_X) / 2)
PIXY_Y_CENTER          =  ((PIXY_MAX_Y-PIXY_MIN_Y) / 2)


# Pixy Python SWIG get blocks example #

# Initialize Pixy Interpreter thread #
pixy_init()

class Blocks (Structure):
  _fields_ = [ ("type", c_uint),
               ("signature", c_uint),
               ("x", c_uint),
               ("y", c_uint),
               ("width", c_uint),
               ("height", c_uint),
               ("angle", c_uint) ]

blocks = BlockArray(100)
frame  = 0

arduino = None

for i in range(0, 4):
    try:
        arduino = serial.Serial('/dev/ttyACM%d'  % i, 19200, writeTimeout = 0)
        break
    except: pass


# Wait for blocks #
while 1:

  count = pixy_get_blocks(100, blocks)

  if count > 0:
    # Blocks found #
    frame = frame + 1
    if (frame % 100000) == 0:
		for index in range (0, count):
			fwdRange = 30
			pan_error = fwdRange + 1
   			if blocks[index].width * blocks[index].height > 400:
				pan_error = PIXY_X_CENTER - blocks[index].x	
			if (pan_error < fwdRange and pan_error > -fwdRange):
				data = {"command":"forward"}
				json_data = json.dumps(data)
			elif (pan_error < -fwdRange):
				data = {"command":"right"}
				json_data = json.dumps(data)
			elif (pan_error > fwdRange):
				data = {"command":"left"}
				json_data = json.dumps(data)

			arduino.write(json_data);
			arduino.write("\n")
			arduino.flush()
			print json_data
  else:
		data = {"command":"stop"}
		json_data = json.dumps(data)
		arduino.write(json_data)
		arduino.write("\n")
		print json_data
