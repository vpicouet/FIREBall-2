#!/usr/bin/python

import signal
import serial
import os
import sys
import time
from datetime import datetime,timedelta
import numpy as np

def set_pow_cmd(ser, cmd):

    endchar='\r\n'

    ser0.write('%s\r' % cmd)
    time.sleep(1)

    reply = ''

    while ser0.inWaiting() > 0: 
	rtd=ser0.read(1)
        reply+=rtd
        if not reply or reply == "exit":
            break

        #if rtd == '\n':
         #  break

    return(reply)

#---------------------------------------------------------------------------
# The main function starts here
#-----------------------------------------------------------------------------
if __name__ == "__main__":

    try:

    # open the serial device
    # set the cryocooler target tempearture TTARGET
    # set the cryocooler power pwout

        ser0  = serial.Serial(port='/dev/ttyS5',
                              baudrate=9600,
                              parity=serial.PARITY_NONE,
                              stopbits=serial.STOPBITS_ONE,
                              bytesize=serial.EIGHTBITS)

        #temp = omega_cmd(ser0, sys.argv[1:])
        #temp = ''.join(map(str,sys.argv[1:]))
        power = ''.join(map(str,sys.argv[1:]))
        #cmd = 'TTARGET=' + temp
        cmd = 'PWOUT=' + power
	#cmd = 'SET SSTOP=0'
        #cmd = 'PWOUT=60'
        output = set_pow_cmd(ser0, cmd)
        print output

    finally:
        
        ser0.close()

