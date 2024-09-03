#!/usr/bin/python
# -----------------------------------------------------------------------------
# @file      tail_temps_press.py
# @brief     read temperature and pressure every hour from current files for Fireball2 instrument
# @author    Gillian Kyne <gkyne@caltech.edu>
# @date      2018-05-30 gkyne@caltech.edu  
#
# -----------------------------------------------------------------------------

import numpy as np
import signal
import serial
import os
import sys
import time
from datetime import datetime,timedelta
import subprocess
from subprocess import Popen, PIPE

# -----------------------------------------------------------------------------
# @brief  the main function starts here
# -----------------------------------------------------------------------------

if __name__ == "__main__":

    try:

	fn    = "/home/fireball2/data/" + datetime.now().strftime("%y%m%d") + "/alltemps.csv"
	fnpress    = "/home/fireball2/data/" + datetime.now().strftime("%y%m%d") + "/waterpress.csv"
	fndis    = "/home/fireball2/data/" + datetime.now().strftime("%y%m%d") + "/discretecom.csv"
	fnpow    = "/home/fireball2/data/" + datetime.now().strftime("%y%m%d") + "/power.csv"
	fnagilent    = "/home/fireball2/data/" + datetime.now().strftime("%y%m%d") + "/pressure.csv"

	fnph    = "/home/fireball2/data/masterflight_hk/alltemps-ph.csv"
	dir   = os.path.dirname(fn)          # fully qualified path

	if not os.path.exists(dir):          # create data directory if needed
	    os.makedirs(dir)

	fnpressph    = "/home/fireball2/data/masterflight_hk/waterpress-ph.csv"
	dir   = os.path.dirname(fnpress)          # fully qualified path

	if not os.path.exists(dir):          # create data directory if needed
	    os.makedirs(dir)

	fndisph    = "/home/fireball2/data/masterflight_hk/discretecom-ph.csv"
	dir   = os.path.dirname(fndis)          # fully qualified path

	if not os.path.exists(dir):          # create data directory if needed
	    os.makedirs(dir)

	fnpowph    = "/home/fireball2/data/masterflight_hk/power-ph.csv"
	dir   = os.path.dirname(fnpow)          # fully qualified path

	if not os.path.exists(dir):          # create data directory if needed
	    os.makedirs(dir)

	fnagilentph    = "/home/fireball2/data/masterflight_hk/pressure-ph.csv"
	dir   = os.path.dirname(fnagilent)

	if not os.path.exists(dir):          # create data directory if needed
	    os.makedirs(dir)


	##Append alltemps.csv to new file
	fp = open(fnph, 'a')
        p = Popen(['tail','-1',fn], stdout=PIPE, stderr=PIPE)
        stdout, stderr = p.communicate()
        fp.write(stdout)
	fp.close()

	##Append waterpress.csv to new file
	fp = open(fnpressph, 'a')         
	p = Popen(['tail','-1',fnpress], stdout=PIPE, stderr=PIPE)
        stdout, stderr = p.communicate()
        fp.write(stdout)
	fp.close()

	##Append discretecom.csv to new file
	fp = open(fndisph, 'a')         
	p = Popen(['tail','-1',fndis], stdout=PIPE, stderr=PIPE)
        stdout, stderr = p.communicate()
        fp.write(stdout)
	fp.close()

	##Append power.csv to new file
	fp = open(fnpowph , 'a')         
	p = Popen(['tail','-1',fnpow], stdout=PIPE, stderr=PIPE)
        stdout, stderr = p.communicate()
        fp.write(stdout)
	fp.close()

	##Append pressure.csv to new file
	fp = open(fnagilentph , 'a')         
	p = Popen(['tail','-1',fnagilent], stdout=PIPE, stderr=PIPE)
        stdout, stderr = p.communicate()
        fp.write(stdout)
	fp.close()


    except:
        'No files to read from....'


