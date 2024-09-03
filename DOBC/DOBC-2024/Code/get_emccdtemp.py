#!/usr/bin/python
# -----------------------------------------------------------------------------
# @file      get_emccdtemp.py
# @brief     read temperature from Fireball2 Omega Controller
# @author    David Hale <dhale@caltech.edu>
# @date      2014-06-27
# @modified  2014-11-11 nicole@srl.caltech.edu  
# @modified  2015-10-30 Gillian Kyne <gkyne@caltech.edu>
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
# @fn     omega_cmd
# @brief  send command to pressure gauge
# @param  ser  serial object
# @param  cmd  command string
# -----------------------------------------------------------------------------
def get_emccdtemp(ser, cmd):

    endchar='\r\n'

    retval = {}

    # send command here
    ser.write('CRDG? %s\r\n' % cmd)
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
    retval[cmd] = float( temp[0] )

    return retval

# -----------------------------------------------------------------------------
# @fn     main
# @brief  the main function starts here
# -----------------------------------------------------------------------------
if __name__ == "__main__":

    try:

        # open the serial devices to Lesker 354 Ion Gauge

        ser0  = serial.Serial(port='/dev/omega',
                              baudrate=9600,
                              parity=serial.PARITY_ODD,
                              stopbits=serial.STOPBITS_ONE,
                              bytesize=serial.SEVENBITS)
 
	# converting setpoint 1 value from string to hex

	#setpoint = hex conversion of sys.argv[1:] 
	#hex command for setpoint + converted number-to-hex inserted below before
	#where is says sys.argv[1:]. sys.argv[1:] is replaced by setpoint above.

        temps = get_emccdtemp(ser0, " ".join(sys.argv[1:]))
        time  = datetime.now().strftime("%D %T")
        dat   = [time, ",", temps, "\n"]     # put data in convenient list

        fn    = "/home/fireball2/data/" + \
                datetime.now().strftime("%y%m%d") + "/temps.csv"
        dir   = os.path.dirname(fn)          # fully qualified path

        if not os.path.exists(dir):          # create data directory if needed
            os.makedirs(dir)

        if not os.path.exists(fn):           # write a header?
            writeheader = True
        else:
            writeheader = False

        fp = open(fn, 'a')                   # open file for append/write

        if writeheader:
            fp.write('time,temp\n')         # header

        fp.write('{0}, {1:.2f}\n'.format(time, temps))

    finally:
        fp.close()
        ser0.close()
