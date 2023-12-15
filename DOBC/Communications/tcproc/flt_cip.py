import time
import serial
import sys

def fltuplink():

	ser_in = serial.Serial('/dev/FLTUplink',
		             baudrate=38400,
		             bytesize=serial.EIGHTBITS,
		             parity=serial.PARITY_NONE,
		             stopbits=serial.STOPBITS_ONE)

	ser_out = serial.Serial('/dev/FLTMuxRx',
		             baudrate=38400,
		             bytesize=serial.EIGHTBITS,
		             parity=serial.PARITY_NONE,
		             stopbits=serial.STOPBITS_ONE)
	while 1 :
	    out = ""
	    while ser_in.inWaiting() > 0: 
			rtd=ser_in.read(size=ser_in.inWaiting())
			out+=rtd
		
	    if out!="":
		    #print ''.join(format(ord(x),'b') for x  in out)
		    #sys.stdout.flush()
		    ser_out.write(out)

def fltdnlink():

	ser_in = serial.Serial('/dev/FLTMuxTx',
                     baudrate=38400,
                     bytesize=serial.EIGHTBITS,
                     parity=serial.PARITY_NONE,
                     stopbits=serial.STOPBITS_ONE)

	ser_out = serial.Serial('/dev/FLTDownlink',
                     baudrate=38400,
                     bytesize=serial.EIGHTBITS,
                     parity=serial.PARITY_NONE,
                     stopbits=serial.STOPBITS_ONE)

	while 1 :
	    out = ""
	    while ser_in.inWaiting() > 0: 
			rtd=ser_in.read(size=ser_in.inWaiting())
			out+=rtd
		
	    if out!="":
			#print ' '.join(format(ord(x),'b') for x  in out)
			ser_out.write(out)

if __name__ == "__main__":

	fltdnlink()
        fltuplink()

