# test_subprocess.py
from subprocess import Popen, PIPE, STDOUT
import sys
import os
import subprocess



filename=sys.argv[0]			# MUST BE FILE NAME/STRING	#"hk_170902_130130.tar.gz"
filename="image_000000.fits.tar.gz"
read, write = os.pipe()

# IF Transmit:
os.write(write, "y\n y\n y\n 11\n 128\n 10\n 2\n %s\n y\n 14\n" % (filename))

# IF Receive:
#~ os.write(write, "y\n y\n y\n 12\n 128\n 10\n 2\n %s\n" % (filename))

os.close(write)

# Run script
p=subprocess.check_call(["./test"],stdin=read)
