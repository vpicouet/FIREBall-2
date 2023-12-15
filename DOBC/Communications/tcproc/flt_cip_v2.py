import time
import serial
import sys
import os
from subprocess import Popen, PIPE, STDOUT 

p = Popen('python /home/fireball2/Documents/Communications/tcprocs/flt_uplink.py',shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
p = Popen('python /home/fireball2/Documents/Communications/tcprocs/flt_dnlink.py',shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)

