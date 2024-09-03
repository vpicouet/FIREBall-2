#!/usr/bin/python
# -----------------------------------------------------------------------------
# @file      set_arduino.py
# @brief     send temperature setpoint to arduino
# @author    Gillian Kyne <gkyne@caltech.edu>
# @date      2017-06-19
# -----------------------------------------------------------------------------

import serial
import time as time
import os,sys
from sys import argv

#ser = serial.Serial(port='/dev/arduino',baudrate=115200)
ser = serial.Serial(port='/dev/ttyUSB0',baudrate=115200)

def readard():

    temp = []
    rtd = ''
    i = 0

    while True:
        if ser.inWaiting() > 0:  #if incoming bytes are waiting to be read from the serial input buffer
            r = ser.read(1) #reads whole line as it's printed from the arduino
            rtd += r
            if r == '\n':
                temp.append(rtd.strip())
                rtd = ''
                i = i+1
                continue
            #if i == 5:
            if i == 4:
                break
    return temp



print readard()
if len(sys.argv) > 1:
    ser.write(sys.argv[1])
    time.sleep(2)
    #print ser.readline()
if len(sys.argv) < 1:
    #print ser.readline()

    ser.close()
