import json
import serial
from pixy import *
from ctypes import *

PIXY_MIN_X             =    0
PIXY_MAX_X             =  319
PIXY_MIN_Y             =    0
PIXY_MAX_Y             =  199
PIXY_X_CENTER          =  ((PIXY_MAX_X-PIXY_MIN_X) / 2)
PIXY_Y_CENTER          =  ((PIXY_MAX_Y-PIXY_MIN_Y) / 2)


# Pixy Python SWIG get blocks example #

print ("Pixy Python SWIG Example -- Get Blocks")

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

ser = serial.Serial("/dev/ttyACM0",9600, timeout=None, writeTimeout=None)

# Wait for blocks #
while 1:

  count = pixy_get_blocks(100, blocks)

  if count > 0:
    # Blocks found #
    frame = frame + 1
    if (frame % 50000) == 0:
      # print 'frame %3d:' % (frame/10000)
      for index in range (0, count):
      		  if blocks[index].width * blocks[index].height > 400:
        		pan_error  = PIXY_X_CENTER - blocks[index].x	
  	  #  if (blocks[index].x > 140 and blocks[index].x < 200 and blocks[index].y > 100 and blocks[index].y < 160 and blocks[index].width > 70 and blocks[index].width < 100 and blocks[index].height > 30 and blocks[index].height < 70):
        		# print '[BLOCK_TYPE=%d SIG=%d X=%3d Y=%3d WIDTH=%3d HEIGHT=%3d error=%3d]' % (blocks[index].type, blocks[index].signature, blocks[index].x, blocks[index].y, blocks[index].width, blocks[index].height, pan_error)
			if (pan_error < 15 and pan_error > -15):
				data = {"command":"forward"}
				json_data = json.dumps(data)
				print json_data
			elif (pan_error < -15):
				data = {"command":"left"}
				json_data = json.dumps(data)
				print json_data
			elif (pan_error > 15):
				data = {"command":"right"}
				json_data = json.dumps(data)
				print json_data
      		  #else:
		#	data = {"command":"right"}
		#	json_data = json.dumps(data)
		#	print json_data
			ser.write(json_data)
			ser.write("\n")
			ser.flush()	
