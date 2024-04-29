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
        cmd = 'status'
        temp = cooler_cmd(ser0, cmd)
        print temp
        test = list(temp)
        newtemp = np.array([test])
        idx = np.where(newtemp == '=')
        idx1 = idx[1]
        test2 = test[idx1[0]+6:]
        test2 = np.array([test2])
        B = test2.reshape(5, 27)
        cooler = []
        cooler1 = ''.join(map(str,B[1,19:24]))
        cooler2 = ''.join(map(str,B[3,19:24]))
        cooler3 = ''.join(map(str,B[4,19:24]))
        cooler3 = str(float(cooler3[:])-273.15)
        
        time  = datetime.now().strftime("%D %T")
        dat   = [time, ",", [cooler1, cooler2, cooler3], "\n"]     # put data in list

        fn    = "/home/fireball2/data/" + datetime.now().strftime("%y%m%d") + "/cooler_temp.csv"
        dir   = os.path.dirname(fn)

        if not os.path.exists(dir):# create data directory if needed
            os.makedirs(dir)

        if not os.path.exists(fn): # write a header?
                writeheader = True
        else:
                writeheader = False

        fp = open(fn, 'a') # open file for append/write

        if writeheader:
            fp.write('time,power[W],reject temp[C],coldhead temp[C]\n')# header
        fp.write('{0}, {1}, {2}, {3}\n'.format(time, cooler1, cooler2, cooler3))
        fp.close()

 	
    finally:
        
        ser0.close()

