#!/usr/bin/python           # This is client.py file

import socket               # Import socket module
import select,sys
import struct

p = select.poll();
p.register(sys.stdin,1);
condition = True

file = open( "/dev/input/mice", "rb" );

def getMouseEvent(s):
  buf = file.read(3);
  button = ord( buf[0] );
  bLeft = button & 0x1;
  bMiddle = ( button & 0x4 ) > 0;
  bRight = ( button & 0x2 ) > 0;
  x,y = struct.unpack( "bb", buf[1:] );
  print ("L:%d, M: %d, R: %d, x: %d, y: %d\n" % (bLeft,bMiddle,bRight, x, y));
  s.send("L:%d, M: %d, R: %d, x: %d, y: %d\n" % (bLeft,bMiddle,bRight, x, y));
  # return stuffs

s = socket.socket();         # Create a socket object
host = '127.0.0.1'; 		 # Get local machine name
port = 12345;                # Reserve a port for your service.

s.connect((host, port));
s.send('confirm connection');
while condition == True:
	print s.recv(20)
	getMouseEvent(s);
	result = p.poll(5);

	if len(result) > 0:
		print 'result'
		read_input = sys.stdin.read(1);
		while len(read_input) > 0 and condition == True:
			if read_input== "q":
				condition = False; #type q and enter to quit the client

s.send('q');
s.shutdown;
s.close;                     # Close the socket when done
file.close();
