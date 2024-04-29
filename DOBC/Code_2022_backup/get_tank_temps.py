#!/usr/bin/python
# -----------------------------------------------------------------------------
# @file      get_pressure.py
# @brief     read pressure from Fireball2 Lesker 354 Ion Gauge
# @author    David Hale <dhale@caltech.edu>
# @date      2014-06-27
# @modified  2014-11-11 nicole@srl.caltech.edu  
#
# -----------------------------------------------------------------------------

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
    raise IOError("SIGALRM timeout")

# -----------------------------------------------------------------------------
# @fn     lks_cmd
# @brief  send command to pressure gauge
# @param  ser  serial object
# @param  cmd  command string
# -----------------------------------------------------------------------------
def get_temps(ser, channels):

    endchar='\r\n'

    retval = {}

    for ch in channels:

        # send command here
        ser.write('CRDG? %s\r\n' % ch)
        time.sleep(0.2)

        # setup a sigalarm timer, disabled only when home complete
        signal.signal(signal.SIGALRM, sighandler)
        signal.alarm(5)

        # poll status, wait for motion to stop
        temp=''
        while True:
            while ser.inWaiting() > 0:      # read chars from device
                r=ser.read(1)
                temp+=r
            if endchar in temp:
                break

        signal.alarm(0)                     # disarm timer

        temp=temp.split(endchar)
        retval[ch] = float( temp[0] )

    return retval

# -----------------------------------------------------------------------------
# @fn     main
# @brief  the main function starts here
# -----------------------------------------------------------------------------
if __name__ == "__main__":

    try:

        # open the serial devices to Lesker 354 Ion Gauge

        ser0  = serial.Serial(port='/dev/lakeshore',
                              baudrate=9600,
                              parity=serial.PARITY_EVEN,
                              stopbits=serial.STOPBITS_ONE,
                              bytesize=serial.SEVENBITS)
 
        temps = get_temps(ser0, ['1','2','3'])
        print temps
        time  = datetime.now().strftime("%D %T")
        dat   = [time, ",", temps, "\n"]     # put data in convenient list

        fn    = "/home/fireball2/data/" + \
                datetime.now().strftime("%y%m%d") + "/temps_tank.csv"
        dir   = os.path.dirname(fn)          # fully qualified path
        #print fn

        if not os.path.exists(dir):          # create data directory if needed
            os.makedirs(dir)

        if not os.path.exists(fn):           # write a header?
            writeheader = True
        else:
            writeheader = False

        fp = open(fn, 'a')                   # open file for append/write

        if writeheader:
            fp.write('time,CuClamp1[C],Getter[C],CuClamp2[C]\n')         # header

        fp.write('{0}, {1:.2f},{2:.2f},{3:.2f}\n'.format(time, temps['1'], temps['2'], temps['3']))

    finally:
        fp.close()
        ser0.close()
