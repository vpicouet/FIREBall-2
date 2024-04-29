import os.path
import os, sys
import numpy as np
from subprocess import call
import sys
import time
from datetime import datetime,timedelta

fn = '/home/fireball2/data/' + datetime.now().strftime("%y%m%d") + "/file_num.txt"
dir = os.path.dirname(fn)	# fully qualified path


if not os.path.exists(dir):          # create data directory if needed
	os.makedirs(dir)

