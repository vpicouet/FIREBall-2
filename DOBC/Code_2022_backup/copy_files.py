import os.path
import os, sys
import numpy as np
import subprocess
from subprocess import call
import sys
import time
from datetime import datetime,timedelta

fn = "/home/fireball2/data/" + datetime.now().strftime("%y%m%d") + "/"
dir = os.path.dirname(fn)	# fully qualified path


def copy_files():

    fp = open(fn + '.logfile','a')
    cmd = 'rsync -ru ' + fn + ' fireball2@131.215.196.22:/home/fireball2/data/FC_LAM/' + datetime.now().strftime("%y%m%d") + "/"
    time  = datetime.now().strftime("%D %T")
    fp.write('Files copied at ' + str(time) + '\n')
    fp.close()
    subprocess.call(cmd,shell=True)
    
# -----------------------------------------------------------------------------
# The main function starts here
# -----------------------------------------------------------------------------
if __name__ == "__main__":
	    
        copy_files()


