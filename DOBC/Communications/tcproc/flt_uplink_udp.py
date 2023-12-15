import time
import serial
import sys
import socket

print "Relaying serial from UDP Uplink to FLT MUX Rx"

port = 50040

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(("", port))
print "Waiting on port:", port

ser_out = serial.Serial('/dev/FLTMuxRx',
                     baudrate=38400,
                     bytesize=serial.EIGHTBITS,
                     parity=serial.PARITY_NONE,
                     stopbits=serial.STOPBITS_ONE)
while 1 :
    out = ""
    out, addr = s.recvfrom(1024)
    #out = data
    print out
    
    if out!="":
	    #print ''.join(format(ord(x),'b') for x  in out)
	    #sys.stdout.flush()
	    ser_out.write(out)


