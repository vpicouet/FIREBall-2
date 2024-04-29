#!/usr/bin/python

import os.path
import os, sys
# import numpy as np
import subprocess
import sys
import time
from datetime import datetime,timedelta
import get_imagenum
import get_exptime
import get_emgain
import reset_emgain
import inc_imagenum
import get_cwd
import fndir

fn = fndir.fndir()
dir = os.path.dirname(fn)	# fully qualified path

def dark(exptime_sky,emgain_sky):

    imagenum = get_imagenum.imagenum()
    exptime = exptime_sky
    emgain = emgain_sky
    cwd = get_cwd.cwd()
   
    #c++ script is passed imagenum, exptime and emgain
    command = 'dark '+str(exptime)+' '+str(emgain)+' '+ str(cwd) +' '+str(imagenum)
    print command
    subprocess.call(command,shell=True)

    print '\n......script is ended...'
    print '\n......the values are....'
    print('\n......exposure was set to..' + str(exptime) + 's')
    print('\n......emgain was set to....' + str(emgain))
    print('\n......recorded image was called: ' + cwd.rstrip('\n\r') + 'image{:06d}'.format(int(imagenum)) + '.fits')

    # may need to apause here to allow exposure to finish? 
    emgain = reset_emgain.reset_emgain()
    imagenum = inc_imagenum.inc_imagenum()

    print('\n......emgain was set to....' + str(emgain))
    print('\n......next image is called.......' + 'image{:06d}'.format(int(imagenum)) + '.fits')


def dark_sky():

    exptime_sky = 0, 1, 10, 100
    total_time = 2, 2, 1, 1
    emgain_sky = 0, 8526, 9155, 9783, 10412, 11040

    for j in range(0,6):
        for i in range(0,4):
            for k in range(0,total_time[i]): 
                dark(exptime_sky[i],emgain_sky[j])
    

# -----------------------------------------------------------------------------
# The main function starts here
# -----------------------------------------------------------------------------
if __name__ == "__main__":

	dark_sky()



