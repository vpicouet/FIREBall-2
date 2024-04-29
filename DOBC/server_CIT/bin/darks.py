#!/usr/bin/python

import os.path
import os, sys
# import numpy as np
import subprocess
from subprocess import Popen, PIPE, STDOUT
import sys
import time
from datetime import datetime,timedelta

def expose(exptime,emgain,logfile):

    command0 = './cam exptime='+str(exptime)
    p = Popen(command0, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
    output = p.stdout.read()
    logfile.write('Exposure time= %s' % str(output))
    #subprocess.call(command1,shell=True)

    command1 = './cam vss'
    p = Popen(command1, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
    output = p.stdout.read()
    logfile.write('VSS= %s' % str(output))
    #subprocess.call(command2,shell=True)
    
    command2 = './cam emgain='+str(emgain)
    p = Popen(command2, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
    output = p.stdout.read()
    logfile.write('EM gain= %s' % str(output))
    #subprocess.call(command2,shell=True)
    
    command3 = './cam expose'
    p = Popen(command3, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
    #subprocess.call(command3,shell=True,stdout=logfile)

    command4 = 'tail -n 1 /home/fireball2/data/' + datetime.now().strftime("%y%m%d") + '/diode1.csv'
    p = Popen(command4, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
    output = p.stdout.read()
    logfile.write('diode1= %s' % str(output))

    command5 = 'tail -n 1 /home/fireball2/data/' + datetime.now().strftime("%y%m%d") + '/omega_temp.csv'
    p = Popen(command5, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
    output = p.stdout.read()
    logfile.write('omega= %s\n\n' % str(output))

    #subprocess.call(command4,shell=True,stdout=logfile)
    #subprocess.call(command5,shell=True,stdout=logfile)

def darks(emgain):

    data_dir = '/home/fireball2/data/' + datetime.now().strftime("%y%m%d") +'/'
    logfile = open(data_dir+'logfile','a')

    exptime_sky = 0, 1, 10, 100, 200, 300, 0
    total_time = 5, 5, 5, 5, 5, 5, 5

    for j in range(0,7):
            for k in range(0,total_time[j]):
                command = './cam imno' 
                p = Popen(command, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
                output = p.stdout.read()
                logfile.write('image%06d.fits\n' % int(output))
                expose(exptime_sky[j],emgain,logfile)

    logfile.close()

# -----------------------------------------------------------------------------
# The main function starts here
# -----------------------------------------------------------------------------
if __name__ == "__main__":

	darks(sys.argv[1])




