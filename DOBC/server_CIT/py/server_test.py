#!/usr/bin/python

import os.path
import os, sys
# import numpy as np
import subprocess
import sys
import time
from datetime import datetime,timedelta

def server():

    exptime = 0
    emgain = 0
   
    command1 = './cam exptime='+str(exptime)
    print command1
    subprocess.call(command1,shell=True)

    command2 = './cam emgain='+str(emgain)
    print command2
    subprocess.call(command2,shell=True)
    
    for i in range(0,200):
       #server code is passed imagenum, exptime and emgain
       command4 = './cam expose'
       print command4
       subprocess.call(command4,shell=True)

# -----------------------------------------------------------------------------
# The main function starts here
# -----------------------------------------------------------------------------
if __name__ == "__main__":

	server()



