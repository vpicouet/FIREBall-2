import time
import serial
import sys
import struct
import socket

print "Relaying serial from FLT Mux Tx to BT Downlink"

ser_in = serial.Serial('/dev/FLTMuxTx',
                     baudrate=38400,
                     bytesize=serial.EIGHTBITS,
                     parity=serial.PARITY_NONE,
                     stopbits=serial.STOPBITS_ONE)

gnd_IP = "172.20.4.150"
port = 50041

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(("", port))
print "Waiting on port:", port

while 1 :
    out = ""
    while ser_in.inWaiting() > 0: 
		rtd=ser_in.read(size=ser_in.inWaiting())
		out+=rtd
		
    if out!="":
		print ' '.join(format(ord(x),'b') for x  in out)
		s.sendto(out,(gnd_IP,port))

