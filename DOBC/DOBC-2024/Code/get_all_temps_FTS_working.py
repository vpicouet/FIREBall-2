#!/usr/bin/python
# -----------------------------------------------------------------------------
# @file      get_all_temps.py
# @brief     read temperature sensors from Fireball2 instrument
# @author    Gillian Kyne <gkyne@caltech.edu>
# @date      2015-12-12
# @modified  2016-02-13 gkyne@caltech.edu  
# @modified  2022-06-01 gillian.kyne@jpl.nasa.gov
# -----------------------------------------------------------------------------

import numpy as np
import signal
import serial
import os
import sys
import time
from datetime import datetime,timedelta

# -----------------------------------------------------------------------------

def get_arduino(ser3):

    ser3.flushInput()
    ser3.flushOutput()

    #using serial read() instead of readline(), readline() stopped working properly.

    temp = []
    rtd = ''
    i = 0

    while True:
         if ser3.inWaiting() > 0:  #if incoming bytes are waiting to be read from the serial input buffer
            r=ser3.read(1) #reads whole line as it's printed from the arduino
            rtd += r
            if r == '\n':
                temp.append(rtd)
                rtd = ''
                i = i+1
                continue
            if i == 4:
                break
    return temp

def cooler_cmd(ser1, cmd):

    endchar='\r\n'

    ser1.write('%s\r' % cmd)
    time.sleep(1)

    reply = ''

    while ser1.inWaiting() > 0:
        rtd=ser1.read(1)
        reply+=rtd
        if not reply or reply == "exit":
            break

    return(reply)


# -----------------------------------------------------------------------------
# @fn     main
# @brief  the main function starts here
# -----------------------------------------------------------------------------
if __name__ == "__main__":

    try:

        #cooler
        ser1 = serial.Serial(port='/dev/ttyS5',baudrate=9600,parity=serial.PARITY_NONE,stopbits=serial.STOPBITS_ONE,bytesize=serial.EIGHTBITS)
        
        #arduino (detector temps)
        ser3  = serial.Serial(port='/dev/arduino',baudrate=115200,timeout=5,xonxoff=False,rtscts=False,dsrdtr=False)
	
	#arduino temperatures
	ard = get_arduino(ser3)
	#print ard
	ard0 = str(ard[0]).decode().strip() #replace("\r\n","")
	print "DOBC",ard0
	ard1 = str(ard[1]).decode().strip() #replace("\r\n","")
	print  "CF ", ard1
	ard2 = str(ard[2]).decode().strip() #replace("\r\n","")
	print  "Get", ard2
	ard3 = str(ard[3]).decode().strip() #replace("\r\n","")
	print  "Det",ard3


        #cooler temperatures and power

        cmd = 'status'
        temp = cooler_cmd(ser1, cmd)
        print temp
        test = list(temp)
        newtemp = np.array([test])
        idx = np.where(newtemp == '=')
        print idx
        idx1 = idx[1]
        test2 = test[idx1[0]+6:]
        test2 = np.array([test2])
        B = test2.reshape(5, 27)
        cooler = []
        cooler1 = ''.join(map(str,B[1,19:24]))
        cooler2 = ''.join(map(str,B[3,19:24]))
        cooler3 = ''.join(map(str,B[4,19:24]))
        cooler3 = str(float(cooler3[:])-273.15)


	time_now  = datetime.now().strftime("%D %T")
	fn    = "/home/fireball2/data/" + datetime.now().strftime("%y%m%d") + "/alltemps.csv"
	dir   = os.path.dirname(fn)          # fully qualified path

	if not os.path.exists(dir):          # create data directory if needed
		os.makedirs(dir)

	if not os.path.exists(fn):           # write a header?
		writeheader = True
	else:
		writeheader = False

	fp = open(fn, 'a')                   # open file for append/write


	if writeheader:

		fp.write('time,DOBC[C],Getter[C],CuClamp2[C],EMCCDBack[C],Coldhead[C],Reject[C]\n') 

        fp.write('{0},{1: <8},{2: <8},{3: <8},{4: <8},{5: <8},{6: <8}\n'.format(time_now,ard0,ard1,ard2,ard3,cooler3,cooler2)) 

	fp.close()




    finally:
        ser1.close()
        ser3.close()

