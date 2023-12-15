import serial
import sys

port = serial.Serial('/dev/ttyS1',baudrate=4800,bytesize=8,parity='N',stopbits=1)

while 1:

	if port.inWaiting()>0:
		
		c = port.read(size=1)
		print c.encode('hex'),
		sys.stdout.flush()
