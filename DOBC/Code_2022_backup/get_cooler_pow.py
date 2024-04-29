#!/usr/bin/python

import signal
import serial
import os
import sys
import time
from datetime import datetime,timedelta
import numpy as np

def cooler_cmd(ser0, cmd):

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

        ser0=serial.Serial(port='/dev/ttyS5',
                              baudrate=9600,
                              parity=serial.PARITY_NONE,
                              stopbits=serial.STOPBITS_ONE,
                              bytesize=serial.EIGHTBITS)

        #temp = omega_cmd(ser0, sys.argv[1:])
        cmd = 'e'
        power = cooler_cmd(ser0, cmd)
        newpower = np.array(list(power.replace("\r\n","")))
        maxpw = ''.join(map(str,newpower[1:7]))
        minpw = ''.join(map(str,newpower[7:13]))
        commpw = ''.join(map(str,newpower[13:19]))
        print maxpw, commpw, minpw
        
        time  = datetime.now().strftime("%D %T")
        dat   = [time, ",", [maxpw, commpw, minpw], "\n"]     # put data in list

        fn    = "/home/fireball2/data/" + datetime.now().strftime("%y%m%d") + "/cooler_power.csv"
        dir   = os.path.dirname(fn)

        if not os.path.exists(dir):# create data directory if needed
            os.makedirs(dir)

        if not os.path.exists(fn): # write a header?
                writeheader = True
        else:
                writeheader = False

        fp = open(fn, 'a') # open file for append/write

        if writeheader:
            fp.write('time,Max_Power[W],Command_Power[W],Min_Power[W]\n')# header
        fp.write('{0}, {1}, {2}, {3}\n'.format(time, maxpw, commpw, minpw))
        fp.close()

 	
    finally:
        
        ser0.close()

