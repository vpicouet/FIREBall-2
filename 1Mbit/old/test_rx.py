#!/usr/bin/env python2

# Quick test to make sure


import os
import subprocess
import sys
import time
from datetime import datetime
#~ from astropy.io import fits


def main():
	if os.name == "posix":
		root = "/home/fireball2/Communications/comsync-r15-port1"
		dir = '/'
	elif os.name == "nt": # windows
		root = "C:\\Users\\User\\Documents\\FIREBall-2 python\\"
		dir = '\\'
	else:
		print "Unrecognized platform, exiting."
		exit(1)


	delay = 1 # Seconds to wait before moving on to next file
	filenum = 0
	program = "./test"		# Executable from test.c

	#Other variables
	timeinit = datetime.now()
	dt = 0		# Delta time from "now" to "current"
	logfile = "filetrack_rxTEST_%02i%02i%02i_%02i%02i.log" % (
		timeinit.year % 100,
		timeinit.month,
		timeinit.day,
		timeinit.hour,
		timeinit.minute
	)
	i = 0


	#Keep running program unless something sets break condition
	while True:
		text = "tmp4test%02i.fits" % (filenum)
		log(logfile, "\n")
		# Log the start time
		current = datetime.now()

		log(logfile, "\nTime: %02i%02i%02i_%02i:%02i:%02i" % (
			current.year%100,
			current.month,
			current.day,
			current.hour,
			current.minute,
			current.second
		))

		# Receive file using data transfer C code
		# 1. Clear receiver port of junk
		log(logfile, "Clearing Receiver for TEST FILE...")
		returncode = comm_clear(program)
		if returncode != 0:
			log(logfile, "Error in clearning; TRY AGAIN")
			continue		# Re-start recevier to try to get next file

		# 2. Run receiver for next file
		log(logfile, "Running Receiver for TEST FILE...")
		returncode = comm_command(program, text)
		if returncode != 0:
			log(logfile, "Error executing command: %s --> Receiving --> %s" % (program, text))
			continue		# Re-start recevier to try to get next file
		elif returncode == 0:
			log(logfile, "SUCCESS! Received file: %s" % (text))
			filenum += 1






# Wrap print+file output together
def log(logfile, string):
	print(string)
	with open(logfile, 'a') as f:
		f.write(string + '\n')



# Method for running comm C-executable to transmit tar files to ground
def comm_command(program, tar_file):
	if os.name == "posix":		# linux: run ./test for transmission
		try:
			timeout = 3		# Set time for delay to check change in file size to exit receiver
			filename = tar_file
			p = subprocess.Popen(
				[program],
				stdin=subprocess.PIPE,
				stdout=subprocess.PIPE,
				bufsize=1)
			pid = p.pid
			###p.stdin.write('w\n')
			p.stdin.write("y\n y\n y\n 12\n 256\n 1\n 2\n %s\n" % (filename))
			# Use this to break from receiver (TRY)
			oldsize = 0
			while True:
				time.sleep(timeout)
				size = os.path.getsize(filename)
				print size
				if size == 0:
					continue
				elif size == oldsize:
					print "file transfer complete"
					os.kill(pid, 9) # 9 or 15
					break
				oldsize = size
				returncode = 0
		except:
			returncode = 1
	elif os.name == "nt": # windows: ./test won't run, so just skip
		print "Running receiver code (Win: simulated): %s" % (tar_file)
		time.sleep(10)
		returncode = 0
	return returncode


# Use to clear/purge Rx before next transmit
def comm_clear(program):
	if os.name == "posix":		# linux: run ./test for transmission
		read, write = os.pipe()
		# IF Transmit:
		os.write(write, "y\n y\n y\n 7\n 14\n")
		os.close(write)
		try:
			p=subprocess.check_call([program],stdin=read)
			returncode = 0
		except:
			returncode = 1

	return returncode



if __name__ == "__main__":
	main()
