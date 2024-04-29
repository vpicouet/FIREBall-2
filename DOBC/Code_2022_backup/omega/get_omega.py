#!/usr/bin/python

import signal
import serial
import os
import sys
import time
from datetime import datetime,timedelta


def omega_cmd(ser, cmd):

    endchar='\r\n'

    ser0.write('%s\r\n' % cmd)
    time.sleep(1)

    reply = ''

    while ser0.inWaiting() > 0: 
	rtd=ser0.read(1)
        reply+=rtd
        if rtd == '\n':
           break

    return(reply[3:-1])

#---------------------------------------------------------------------------
# The main function starts here
#-----------------------------------------------------------------------------
if __name__ == "__main__":

    try:

    # open the serial device

        ser0  = serial.Serial(port='/dev/omega',
                              baudrate=9600,
                              parity=serial.PARITY_ODD,
                              stopbits=serial.STOPBITS_ONE,
                              bytesize=serial.SEVENBITS)

        #temp = omega_cmd(ser0, sys.argv[1:])
        cmd = '*X01'
        temp = omega_cmd(ser0, cmd)
	if not temp == '':
		print temp

		time  = datetime.now().strftime("%D %T")
		dat   = [time, ",", temp, "\n"]     # put data in list

		fn    = "/home/fireball2/data/" + datetime.now().strftime("%y%m%d") + "/omega_temp.csv"
		dir   = os.path.dirname(fn) 

		if not os.path.exists(dir):# create data directory if needed
		    os.makedirs(dir)

		if not os.path.exists(fn): # write a header?
		        writeheader = True
		else:
			writeheader = False

		fp = open(fn, 'a') # open file for append/write

		if writeheader:
		    fp.write('time,temp[C]\n')# header
		fp.write('{0}, {1}\n'.format(time, temp))
		fp.close()

    finally:
        
        ser0.close()

