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
from datetime import datetime,timedelta

# -----------------------------------------------------------------------------
# @fn     sighandler
# @brief  signal handler
# -----------------------------------------------------------------------------
def sighandler(signum, frame):
    print "sighandler got signal", signum
    #raise IOError("SIGALRM timeout")

#---------------------------------------------------------------------------
def get_temps(ser0, channels):

    endchar='\r\n'

    retval = {}

    for ch in channels:

        # send command here
        ser0.write('CRDG? %s\r\n' % ch)
        time.sleep(0.2)

        # setup a sigalarm timer, disabled only when home complete
        signal.signal(signal.SIGALRM, sighandler)
        signal.alarm(5)

        # poll status, wait for motion to stop
        temp=''
        while True:
            while ser0.inWaiting() > 0:      # read chars from device
                r=ser0.read(1)
                temp+=r
            if endchar in temp or temp == '':
                break

        signal.alarm(0)                     # disarm timer

        if not temp:
            retval[ch] = 'NAN'
	    print 'Lakeshore is not connected...'
        if temp:
	    temp=temp.split(endchar)
	    retval[ch] = float( temp[0] )

    return retval

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

def get_tankarduino1(ser4):

    #using serial read() instead of readline(), readline() stopped working properly.

    temp = []
    rtd = ''
    i = 0

    while True:
         if ser4.inWaiting() > 0:  #if incoming bytes are waiting to be read from the serial input buffer
            r=ser4.read(1) #reads whole line as it's printed from the arduino
            rtd += r
            if r == '\n':
                temp.append(rtd)
                rtd = ''
                i = i+1
                continue
            if i == 4:
                break
    return temp

def get_tankarduino2(ser5):

    #using serial read() instead of readline(), readline() stopped working properly.

    temp = []
    rtd = ''
    i = 0

    while True:
         if ser5.inWaiting() > 0:  #if incoming bytes are waiting to be read from the serial input buffer
            r=ser5.read(1) #reads whole line as it's printed from the arduino
            rtd += r
            if r == '\n':
                temp.append(rtd)
                rtd = ''
                i = i+1
                continue
            if i == 4:
                break
    return temp

#this version stopped working..not sure why. something to do with when temp reading was single digit before decimal, or negative. will investigate at some point
def old_get_arduino(ser3):

    r = ''
    rtd = []

    while True:
         if ser3.inWaiting() > 0:  #if incoming bytes are waiting to be read from the serial input buffer
            r=ser3.readline() #reads whole line as it's printed from the arduino
            #print r[0:10]
            rtd.append(r[0:10])
            #print rtd
            #print len(rtd)
            if len(rtd) == 2: #arduino continuously prints out so this breaks out of the loop
                break

    return rtd


def get_water(ser6):

    #ser6.flushInput()
    #ser6.flushOutput()

    #using serial read() instead of readline(), readline() stopped working properly.

    temp = []
    rtd = ''
    i = 0

    while True:
         if ser6.inWaiting() > 0:  #if incoming bytes are waiting to be read from the serial input buffer
            r=ser6.read(1) #reads whole line as it's printed from the arduino
            rtd += r
            if r == '\n':
                temp.append(rtd)
                rtd = ''
                i = i+1
                break
 
    return temp


def omega_cmd(ser2, omegacmd):

    ser2.write('%s\r\n' % omegacmd)
    time.sleep(1)

    reply = ''

    while ser2.inWaiting() > 0:
        rtd=ser2.read(1)
        reply+=rtd
        if rtd == '\n':
           break

    return(reply[3:-1])

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

        #if rtd == '\n':
         #  break

    return(reply)



# -----------------------------------------------------------------------------
# @fn     main
# @brief  the main function starts here
# -----------------------------------------------------------------------------
if __name__ == "__main__":

    try:

        # open the serial devices

        ser0  = serial.Serial(port='/dev/lakeshore',
                              baudrate=9600,
                              parity=serial.PARITY_EVEN,
                              stopbits=serial.STOPBITS_ONE,
                              bytesize=serial.SEVENBITS)
 
        # cryocooler
        #ser1  = serial.Serial(port='/dev/ttyS0',
        ser1  = serial.Serial(port='/dev/ttyS5',
                              baudrate=9600,
                              parity=serial.PARITY_NONE,
                              stopbits=serial.STOPBITS_ONE,
                              bytesize=serial.EIGHTBITS)

#        ser2  = serial.Serial(port='/dev/omega',
#                              baudrate=9600,
#                              parity=serial.PARITY_ODD,
#                              stopbits=serial.STOPBITS_ONE,
#                              bytesize=serial.SEVENBITS)

        ser3  = serial.Serial(port='/dev/arduino',baudrate=115200,timeout=5,xonxoff=False,rtscts=False,dsrdtr=False)

        #ser4  = serial.Serial(port='/dev/tankarduino1',baudrate=115200,timeout=5,xonxoff=False,rtscts=False,dsrdtr=False)

	#ser5  = serial.Serial(port='/dev/tankarduino2',baudrate=115200,timeout=5,xonxoff=False,rtscts=False,dsrdtr=False)

	#ser6  = serial.Serial(port='/dev/waterdewar',baudrate=115200,timeout=5,xonxoff=False,rtscts=False,dsrdtr=False)

        while True:

            #Lakeshore temperatures
            lkstemp = get_temps(ser0, ['1','2'])
	    print lkstemp
	
            time.sleep(2)
            #arduino temperatures
            ard = get_arduino(ser3)
            #print ard
            ard0 = str(ard[0]).decode().strip() #replace("\r\n","")
            print ard0
            ard1 = str(ard[1]).decode().strip() #replace("\r\n","")
            print ard1
            ard2 = str(ard[2]).decode().strip() #replace("\r\n","")
            print ard2
            ard3 = str(ard[3]).decode().strip() #replace("\r\n","")
            print ard3



            time.sleep(2)
            #Cooler temperatures
            cmd = 'status'
            temp = cooler_cmd(ser1, cmd)
            test = list(temp)
            if test:
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
                print cooler2,cooler3
            if not test:
                cooler1 = 'NAN'
                cooler2 = 'NAN'
                cooler3 = 'NAN'
                
            #Cooler power
            cmd = 'e'
            power = cooler_cmd(ser1, cmd)
            if power :
                newpower = np.array(list(power.replace("\r\n","")))
                maxpw = ''.join(map(str,newpower[1:7]))
                minpw = ''.join(map(str,newpower[7:13]))
                commpw = ''.join(map(str,newpower[13:19]))
                print maxpw, commpw, minpw
            else:
                maxpw = 'NAN'
                minpw = 'NAN'
                commpw = 'NAN'

#########################################################

            time_now  = datetime.now().strftime("%D %T")
            #dat   = [time, ",", [lkstemp, ard0, cooler1, cooler2, cooler3, ard1], "\n"]     # put data in convenient list
		
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
 
	        fp.write('time,CuClamp1[C],Getter[C],CuClamp2[C],EMCCDBack[C],Reject[C],Coldhead[C],bot-tank-lks[C],top-tank-lks[C]\n') 

	    fp.write('{0},{1: <8},{2: <8},{3: <8},{4: <8},{5: <8},{6: <8},{7: <8},{8: <8}\n'.format(time_now,ard0,ard1, ard2, ard3,cooler2,cooler3,lkstemp['1'],lkstemp['2'])) 

            fp.close()



            fnpow    = "/home/fireball2/data/" + datetime.now().strftime("%y%m%d") + "/power.csv"
            dir   = os.path.dirname(fnpow)          # fully qualified path

            if not os.path.exists(dir):          # create data directory if needed
                os.makedirs(dir)

            if not os.path.exists(fnpow):           # write a header?
                writeheader = True
            else:
                writeheader = False

            fp = open(fnpow, 'a')                   # open file for append/write

            if writeheader:
                fp.write('time,MaxCoolerPower[W],MinCoolerPower[W],CommandCoolerPower[W]\n')         # header

            fp.write('{0},{1: <8},{2: <8},{3: <8}\n'.format(time_now, maxpw, minpw, commpw))
            fp.close()
            time.sleep(2)



    finally:
        ser0.close()
        ser1.close()
        #ser2.close()
        ser3.close()
        #ser4.close()
	#ser5.close()
        #ser6.close()
