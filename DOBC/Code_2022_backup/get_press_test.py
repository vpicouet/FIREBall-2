#!/usr/bin/python

import signal
import serial
import os
import sys
import time
from datetime import datetime,timedelta


def press_cmd(ser0, cmd):

    ser0.write('%s\r' % cmd)
    time.sleep(1)

    reply = ''

    while ser0.inWaiting() > 0:
        rtd=ser0.read(1)
        reply+=rtd
        if rtd == '\n':
           break
    
    return(reply)

#---------------------------------------------------------------------------
# The main function starts here
#-----------------------------------------------------------------------------
if __name__ == "__main__":

    try:

    # open the serial device

        ser0  = serial.Serial(port='/dev/ttyS1',
                              baudrate=9600,
                              parity=serial.PARITY_NONE,
                              stopbits=serial.STOPBITS_ONE,
                              bytesize=serial.EIGHTBITS)
        ser0.timeout = 1.5
        ser0.xonxoff = False
        ser0.rtscts = False
        ser0.dsrdtr = False

        #temp = omega_cmd(ser0, sys.argv[1:])
        #print 'Querying the pump pressure...\n'
        result = int('80',16)^int('32',16)^int('32',16)^int('34',16)^int('30',16)^int('03',16)
        cksum = '{:x}'.format(result)
        #cksum =  cksum.encode('hex')
        #cmd = '0x02-0x80-0x32-0x30-0x35-0x30-0x03-0x' + cksum[0:2] + '-0x' + cksum[2:4]
        #cmd = 'x02x85x32x30x33x30x03x38x35'
        cmd = chr(0x2)+chr(0x80)+"224"+"0"+chr(0x3)+cksum
        #print 'The command sent was ' + cmd
        output = press_cmd(ser0, cmd)
	press = temp[5:-5]
        print press
        #print 'The command should have printed..\n'

        time  = datetime.now().strftime("%D %T")
        dat   = [time, ",", press, "\n"]     # put data in list

        fn    = "/home/fireball2/data/" + datetime.now().strftime("%y%m%d") + "/pressure.csv"
        dir   = os.path.dirname(fn)

        if not os.path.exists(dir):# create data directory if needed
            os.makedirs(dir)

        if not os.path.exists(fn): # write a header?
                writeheader = True
        else:
                writeheader = False

        fp = open(fn, 'a') # open file for append/write

        if writeheader:
            fp.write('time,pressure[mbar]\n')# header
        fp.write('{0}, {1}\n'.format(time, press))
        fp.close()


	
    finally:
        
        ser0.close()

