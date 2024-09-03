import os.path
import os, sys
import numpy as np
from subprocess import call
import sys
import time
from datetime import datetime,timedelta

fn = '/home/fireball2/Code/'
dir = os.path.dirname(fn)	# fully qualified path

file_in = open(fn + '/file_num.txt', 'r')
line = file_in.readline()
file_in.close()

print('\nThe previous image is image' + '{:06d}'.format(int(line)) + '.fits')

os.remove(fn + '/file_num.txt')

file_in = open(fn + '/file_num.txt','w')
newline = str(int(line) + 1)
file_in.write(newline)
file_in.close()

newimage = 'image' + '{:06d}'.format(int(newline))

print('\nThe acquired image is image' + '{:06d}'.format(int(newline)) + '.fits')
