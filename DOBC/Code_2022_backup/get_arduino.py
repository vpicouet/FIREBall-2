#!/usr/bin/python
# -----------------------------------------------------------------------------
# @file      get_arduino.py
# @brief     read temperature from Fireball2 ruggeduino RTD shield combo
# @author    Gillian Kyne <gkyne@caltech.edu>
# @date      2016-11-21
# -----------------------------------------------------------------------------

import signal
import serial
import os
import sys
import time
from datetime import datetime,timedelta
import numpy as np

# -----------------------------------------------------------------------------
# @fn     ard_cmd
# @brief  send command to pressure gauge
# @param  ser  serial object
# @param  cmd  command string
# -----------------------------------------------------------------------------

def get_arduino(ser0):

    r = ''
    rtd = []

    while True:
         if ser0.inWaiting() > 0:  #if incoming bytes are waiting to be read from the serial input buffer
            r=ser0.readline() #reads whole line as it's printed from the arduino
            #print r[0:10]
            rtd.append(r[0:10])
            #print rtd
            #print len(rtd)
            if len(rtd) == 2: #arduino continuously prints out so this breaks out of the loop
                break      
    return rtd

# -----------------------------------------------------------------------------
# @fn     main
# @brief  the main function starts here
# -----------------------------------------------------------------------------
if __name__ == "__main__":

	# open the serial port to RTD arduino
	#ser0  = serial.Serial(port='/dev/arduino',baudrate=115200)
        ser0 = serial.Serial(port='/dev/ttyUSB0',baudrate=115200) #,timeout=5,xonxoff=False,rtscts=False,dsrdtr=False)#parity=serial.PARITY_NONE,stopbits=serial.STOPBITS_ONE,bytesize=serial.EIGHTBITS)


        temps = get_arduino(ser0)
        print temps
	time = datetime.now().strftime("%D %T")
	dat = [time, ",", temps, "\n"]     # put data in convenient list

	fn    = "/home/fireball2/data/" + datetime.now().strftime("%y%m%d") + "/rtd.csv"
	dir   = os.path.dirname(fn)          # fully qualified path
	if not os.path.exists(dir):          # create data directory if needed
	    os.makedirs(dir)

	if not os.path.exists(fn):           # write a header?
            writeheader = True
	else:
	    writeheader = False

	fp = open(fn, 'a')                   # open file for append/write
        if writeheader:
	    fp.write('time,temp[C]\n')         # header

	fp.write('{0}, {1:.4f}\n'.format(time, float(temps)))

        fp.close()
        ser0.close()
