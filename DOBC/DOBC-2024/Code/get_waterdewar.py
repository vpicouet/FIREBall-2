#!/usr/bin/python
# -----------------------------------------------------------------------------
# @file      get_all_temps.py
# @brief     read temperature sensors from Fireball2 instrument
# @author    Gillian Kyne <gkyne@caltech.edu>
# @date      2015-12-12
# @modified  2016-02-13 gkyne@caltech.edu  
#
# -----------------------------------------------------------------------------

import numpy as np
import signal
import serial
import os
import sys
import time
import re
from datetime import datetime,timedelta

# -----------------------------------------------------------------------------
# @fn     sighandler
# @brief  signal handler
# -----------------------------------------------------------------------------
def sighandler(signum, frame):
    print "sighandler got signal", signum
    #raise IOError("SIGALRM timeout")

#---------------------------------------------------------------------------

def get_arduino(ser0):

    #using serial read() instead of readline(), readline() stopped working properly.

    temp = []
    rtd = ''
    i = 0

    while True:
         if ser0.inWaiting() > 0:  #if incoming bytes are waiting to be read from the serial input buffer
            r=ser0.read(1) #reads whole line as it's printed from the arduino
            rtd += r
            if r == '\n':
                temp.append(rtd)
                rtd = ''
                i = i+1
                break
    return temp

# -----------------------------------------------------------------------------
# @fn     main
# @brief  the main function starts here
# -----------------------------------------------------------------------------
if __name__ == "__main__":

    try:

        # open the serial devices

        ser0  = serial.Serial(port='/dev/waterdewar',baudrate=115200,timeout=5,xonxoff=False,rtscts=False,dsrdtr=False)

        time.sleep(2)
        #arduino temperatures
        ard = get_arduino(ser0)
        #print ard
        ard0 = str(ard[0]).decode().strip() #replace("\r\n","")
        line = ard0.replace(';',',')
	print str(line)
        press1 = str(line.split(',')[0])
        temp1 = str(line.split(',')[1])
        press2 = str(line.split(',')[2])
        temp2 = str(line.split(',')[3])
        temp3 = str(line.split(',')[4])
	temp4 = str(line.split(',')[5])
        discrete1 = str(line.split(',')[6])

        time  = datetime.now().strftime("%D %T")
		
        fn    = "/home/fireball2/data/" + datetime.now().strftime("%y%m%d") + "/waterdewar.csv"
        dir   = os.path.dirname(fn)          # fully qualified path

        if not os.path.exists(dir):          # create data directory if needed
            os.makedirs(dir)

	if not os.path.exists(fn):           # write a header?
            writeheader = True
        else:
            writeheader = False

        fp = open(fn, 'a')                   # open file for append/write

        if writeheader:
            fp.write('time,Pressure-bot[mbar],Water-Temp-bot[C],Pressure-air[mbar],MUX-Temp[C],Water-Temp-bot-pt100[C],Water-Temp-top[C]\n') 

	fp.write('{0},{1: <8},{2: <8},{3: <8},{4: <8},{5: <8},{6: <8}\n'.format(time,press1,temp1,press2,temp2,temp3,temp4))
        
        fp.close()

        fn    = "/home/fireball2/data/" + datetime.now().strftime("%y%m%d") + "/discretecom.csv"
        dir   = os.path.dirname(fn)          # fully qualified path

        if not os.path.exists(dir):          # create data directory if needed
            os.makedirs(dir)

	if not os.path.exists(fn):           # write a header?
            writeheader = True
        else:
            writeheader = False

        fp = open(fn, 'a')                   # open file for append/write

        if writeheader:
            fp.write('time,Discrete-Command\n') 

	fp.write('{0},{1: <8}\n'.format(time,discrete1))
        
        fp.close()

        
    finally:
        ser0.close()
        
