#!/usr/bin/python

import signal
import serial
import os
import sys
import time
from datetime import datetime,timedelta

 
def omega_cmd(ser, cmd):

    ser0.write('%s\r\n' % cmd)
    time.sleep(0.2)
    rtd=ser0.read(10)
#ser0.close()

    return(rtd)

#---------------------------------------------------------------------------
# The main function starts here
#-----------------------------------------------------------------------------
if __name__ == "__main__":

    try:

    # open the serial device

        ser0  = serial.Serial(port='/dev/omega',
                              baudrate=9600,
                              parity=serial.PARITY_ODD,
                              stopbits=serial.STOPBITS_ONE,
                              bytesize=serial.SEVENBITS)



        temp = omega_cmd(ser0, '*R02')
	tempBin = "{0:b}".format(int(temp[6:],16))
        tempDec = int(tempBin,2)
	
	
        tempDec = tempDec/float(10)
	
	temp_SP2 = ''

        if temp[3] == 2:
            temp_SP2 = str(tempDec) + 'C'

	elif temp[3] == 'A':
	    temp_SP2 = '-' + str(tempDec) + 'C'
	

	print 'Setpoint 2 is currently set to ' + temp_SP2


    finally:
        
        time.sleep(1)
        ser0.close()

