#!/usr/bin/python
# -----------------------------------------------------------------------------
# @file      get_pressure.py
# @brief     read pressure from Fireball2 Lesker 354 Ion Gauge
# @author    David Hale <dhale@caltech.edu>
# @date      2014-06-27
# @modified  2014-06-27  
#
# -----------------------------------------------------------------------------

import signal
import serial
import os
import shutil
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
# @fn     ion_cmd
# @brief  send command to pressure gauge
# @param  ser  serial object
# @param  cmd  command string
# -----------------------------------------------------------------------------
def ion_cmd(ser, cmd):

    # send command here
    ser.write(cmd)
    time.sleep(0.1)

    # setup a sigalarm timer, disabled only when home complete
    #signal.signal(signal.SIGALRM, sighandler)
    #signal.alarm(5)

    # poll status, wait for motion to stop
    out=''
    while len(out)<13:
        while ser.inWaiting() > 0:      # read chars from device
            r=ser.read(1)
            out+=r
    #signal.alarm(0)                     # disarm timer

    out=out.split(' ')                  # separate command from ID

    return out[1]

# -----------------------------------------------------------------------------
# @fn     main
# @brief  the main function starts here
# -----------------------------------------------------------------------------
if __name__ == "__main__":

    #try:

        # open the serial devices to Lesker 354 Ion Gauge

        ser0  = serial.Serial(port='/dev/iongauge',
                              baudrate=19200,
                              parity=serial.PARITY_NONE,
                              stopbits=serial.STOPBITS_ONE,
                              bytesize=serial.EIGHTBITS)

        res = ion_cmd(ser0, '#01IG0\r') #IG needs to be powered OFF first to clear an error
        
        if res == 'PROGM':
            time.sleep(3)
            print 'Ion Gauge OFF'
        else:
            print 'ERROR'

        igon = ion_cmd(ser0, '#01IG1\r')     # command to turn IG ON

        if igon == 'PROGM':
            time.sleep(3)
            print 'Ion Gauge ON'
        else:
            print 'ERROR'

        ser0.close()
