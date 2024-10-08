#!/usr/bin/python
# -----------------------------------------------------------------------------
# @file      get_pressure.py
# @brief     read pressure from Fireball2 Lesker 354 Ion Gauge
# @author    David Hale <dhale@caltech.edu>
# @date      2014-06-27
# @modified  2016-02-13 <gkyne@caltech.edu> 
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
    signal.signal(signal.SIGALRM, sighandler)
    signal.alarm(5)

    # poll status, wait for motion to stop
    out=''
    while len(out)<13:
        while ser.inWaiting() > 0:      # read chars from device
            r=ser.read(1)
            out+=r
    signal.alarm(0)                     # disarm timer

    out=out.split(' ')                  # separate command from ID

    return out[1]

#--------------------------------------------------------------
# @fn     main
# @brief  the main function starts here
# -----------------------------------------------------------------------------
if __name__ == "__main__":

    try:

        # open the serial devices to Lesker 354 Ion Gauge

        ser0  = serial.Serial(port='/dev/iongauge',
                              baudrate=19200,
                              parity=serial.PARITY_NONE,
                              stopbits=serial.STOPBITS_ONE,
                              bytesize=serial.EIGHTBITS)
 
        press = ion_cmd(ser0, '#01RD\r')     # command to read pressure
        print press
        time  = datetime.now().strftime("%D %T")
        dat   = [time, ",", press, "\n"]     # put data in convenient list
        htmsrc= "/home/fireball2/Code/index-fireball.html"
        dysrc = "/home/fireball2/Code/dygraph-combined.js"
        dylink= "/home/fireball2/data/" + \
                datetime.now().strftime("%y%m%d") + "/dygraph-combined.js"
        html  = "/home/fireball2/data/" + \
                datetime.now().strftime("%y%m%d") + "/index.html"
        fn    = "/home/fireball2/data/" + \
                datetime.now().strftime("%y%m%d") + "/press.csv"
        dir   = os.path.dirname(fn)          # fully qualified path

        if not os.path.exists(dir):          # create data directory if needed
            os.makedirs(dir)

        if not os.path.exists(html):         # copy index.html?
            shutil.copyfile(htmsrc, html)

        if not os.path.exists(dylink):       # create symlink to dygraph?
            os.symlink(dysrc, dylink)

        if not os.path.exists(fn):           # write a header?
            writeheader = True
        else:
            writeheader = False

        fp = open(fn, 'a')                   # open file for append/write

        if writeheader:
            fp.write('time,pressure[mbar]\n')         # header

        fp.write("".join(dat))               # data

    finally:
        fp.close()
        ser0.close()
