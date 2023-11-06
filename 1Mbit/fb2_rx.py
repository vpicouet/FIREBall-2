# Same as fb2_rx_v1.py but with a fixed date as the input. FLIGHT CODE.
# MUST be run through virtualenv!:
#
# voldy:/home/fireball2/Communications/comsync-r15-port1/python_env/bin/python
#	(or in the folder: python_env/bin/python )
#
#
#
# Handles recieving a new compressed file with either the new image or houskeeping files
# on the GSE during flight and de-compresses them, remains them, and puts them in appropriate
# directories:
#			- Turn receiver on to receive new file: tmp.tar.gz
#			- Close the receiver
#			- De-compress the tar file
#      - IF DE-COMPRESSION FAILS:
#						- Make note in logfile of the time or receiving file and continue
#			- IF DE-COMPRESSION PASSES:
#						- Use size of tar to determine where the files need to go (if < 1 MB, houekeeping file likely)
#						- Place file in correct destination path
#						- IF FILE IS IMAGE: Open image, pull filename from header, and rename file to header.fits
#      - Turn receiver on, wait for next file
#
#
##
### MAJOR CHANGES:
##
#		- Files transmitted are no long compressed!
#		- Files need to be moved now through python os.Popen() command
#		- FITS files are now transmitted as TXT files --> change file extension
#

import os
import subprocess
import sys
import tarfile
import time
import numpy
from datetime import datetime, timedelta
from astropy.io import fits


def main():
	if os.name == "posix":
		root = "/home/fireball2/Communications/comsync-r17-port1"
		dir = '/'
	elif os.name == "nt": # windows
		root = "C:\\Users\\User\\Documents\\FIREBall-2 python\\"
		dir = '\\'
	else:
		print "Unrecognized platform, exiting."
		exit(1)

	directories = {
		#~ "hk": "170929",
		#~ "img": "170929"+dir+"images"
		"hk": "/home/fireball2/dobc/data/",
		"img": "/home/fireball2/dobc/data/",
	}
	field_directories = {
		1: "field1",
		2: "field2",
		3: "field3",
		4: "field4",
	}

	for key, value in directories.iteritems():
		directories[key] = os.path.normpath(os.path.join(root, value))+dir

	filename_root = "image"
	housekeeping = [
		#"alltemps.csv",
		#"cooler_power.csv",
		#"discretecom.csv",
		#"waterpress.csv",
	]

	delay = 1 # Seconds to wait before moving on to next file
	print "Arguments called: ", sys.argv[1:]
	num0 = 0
	program = "./test"		# Executable from test.c
	date_folder = "230925"
	#date_folder = datetime.now().strftime("%y%m%d") # date folder is when the script is started, will also change it to hard code the date prior to flight. Need to test.

	#Other variables
	timeinit = datetime.now()
	dt = 0		# Delta time from "now" to "current"
	logfile = "filetrack_rx_%02i%02i%02i_%02i%02i.log" % (
		timeinit.year % 100,
		timeinit.month,
		timeinit.day,
		timeinit.hour,
		timeinit.minute
	)

	filenum = num0


	#Keep running program unless something sets break condition
	while True:
		#~ for key, value in directories.iteritems():
			#~ directories[key] = os.path.join(value, datetime.now().strftime("%y%m%d"))
			#~ print directories[key]
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
		# Start a counter: We will be transmitting down 5 files in a row:
		# 1. - Image (.fits)
		# 2.--5. - Housekeeping (.csv)
		for i in range(0,len(housekeeping)+1):
			if i == 0:		#Transmitting down fits
				# Define filename
				# filename = "%s%06i.txt" % (filename_root, filenum)
				filename = "image.txt"
				log(logfile, "Running Receiver for FIREBall image (TEXT) to %s..." % (filename))
				returncode = comm_command(program, filename)

				if returncode != 0:
					log(logfile, "Error executing command: %s --> Receiving --> %s" % (program, filename))
					break	# Re-start recevier to try to get next file

				# Define destination path to move the new file
				destPath = directories['img']+date_folder
                                # print destPath
                                # print directories['img']
				# Move file to destPath
				returncode = move_file(filename,destPath,logfile)

				if returncode != 0:
					log(logfile, "Error: Moving file %s --> %s FAILED" % (filename,destPath))
					# NBD - just keep going

				filenum += 1


			else:		# Housekeeping files
				# Define filename
				filename = housekeeping[i-1]
				log(logfile, "Running Receiver for FIREBall housekeeping %s..." % (filename))
				returncode = comm_command(program, filename)
				if returncode != 0:
					log(logfile, "Error executing command: %s --> Receiving --> %s" % (program, filename))
					continue		# Re-start recevier to try to get next file

				# Define destination path to move the new file
				destPath = directories['hk']+date_folder
				# Move file to destPath
				returncode = move_file(filename,destPath,logfile)

				if returncode != 0:
					log(logfile, "Error: Moving file %s --> %s FAILED" % (filename,destPath))
					# NBD - just keep going
		##### PURGE COULD FREEZE UP SYSTEM; JUST SLEEP A BIT
		# print "Resting... %i seconds" % (delay)
		# time.sleep(delay)
		# print "Continuing transfer with %s%06i.txt" % (filename_root, filenum)




# Wrap print+file output together
def log(logfile, string):
	print(string)
	with open(logfile, 'a') as f:
		f.write(string + '\n')




# Method for running comm C-executable to transmit tar files to ground
def comm_command(program, filename):
	if os.name == "posix":		# linux: run ./test for transmission
		try:
			timeout = 3		# Set time for delay to check change in file size to exit receiver
			packet = 256            # 256 or 512
			print "Running 1 Mbit transfer..."
			p = subprocess.Popen(
				[program],
				stdin=subprocess.PIPE,
				stdout=subprocess.PIPE,
				bufsize=1)
			pid = p.pid
			###p.stdin.write('w\n')
			p.stdin.write("y\n y\n y\n 12\n %i\n 1\n 2\n %s\n" % (packet, filename))
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
			print "Unable to open program."
			returncode = 1
	elif os.name == "nt": # windows: ./test won't run, so just skip
		print "Running receiver code (Win: simulated): %s" % (tar_file)
		time.sleep(10)
		returncode = 0
	return returncode




# Method for running comm C-executable to transmit tar files to ground
def comm_purge(program):
	if os.name == "posix":		# linux: run ./test for transmission
		read, write = os.pipe()
		# IF Transmit:
		os.write(write, "y\n y\n y\n 7\n 14\n")
		os.close(write)
		try:
			p=subprocess.check_call([program],stdin=read)
			returncode = 0
			time.sleep(5)		# Wait for purge to pass and not block up receiver?
		except:
			returncode = 1
	elif os.name == "nt": # windows: ./test won't run, so just skip
		print "Running transmittion code (Win: simulated): %s" % (tar_file)
		time.sleep(10)
		returncode = 0
	return returncode





def move_file(file,destPath,logfile):
	'''
	Move the file from where it gets transmitted to to appropriate folder.
	'''
	if not os.path.exists(destPath):
		os.makedirs(destPath)
	try:
		os.rename(file,os.path.join(destPath,file))
		# Try to write to fits file
		im = write_fits(os.path.join(destPath,file), destPath, logfile)
		returncode = 0
	except:
		print "File move to %s/%s FAILED" % (destPath,file)
		returncode = 1
	return returncode






def read_fits(filename):
	# Open fits with astropy
	hdulist = fits.open(filename)
	# CHANGE TO CORRECT CALLABLE ONCE KNOWN
	hrd = hdulist[0].header		# Can do hdr.comments[' '] to check header comments
	hdulist.close()
	return hdr






def write_fits(filename, destPath, logfile):
	# Open fits with astropy
	# Haven't tested this mode so there might be errors
	# img = numpy.zeros( shape=[2069,3216] )
	img = numpy.zeros( shape=[2069,2240] )
	if filename.endswith('.fits'):
		hdulist = fits.open(filename,skip_header=2)
		hdulist.info()
		data = hdulist[0].data
		hrd = hdulist[0].header		# Can do hdr.comments[' '] to check header comments
		hdulist.close()
	# Else, if a text file, open with numpy.loadtxt
	elif filename.endswith(".txt"):
		# Define header captions here
		hrd_caps = ['DATE',
								#'IMG',
								'EXPTIME',
								'EMGAIN',
								#'PREAMP',
								'IMNO', 
								'TEMPA',
								'TEMPB',
								'TEMPC',
								'TEMPD'
							]
		# Try to read in min value from file
		try:
			lines = open(filename).readlines()
			fields_headers = lines[0].split()
			hrds = fields_headers[3:]
			print hrds
			fields1 = lines[2].split()
			fields2 = lines[3].split()
			fields3 = lines[4].split()
			min1 = fields1[1]
			min2 = fields2[1]
			min3 = fields3[1]
			if min1 == min2 or min1 == min3:
				min = numpy.int_(min1)
			elif min2 == min3:
				min = numpy.int_(min2)
			else:
				min = numpy.int_(min3)
			if not (min1 == min2 == min3):
				raise ValueError("****CHECK: min value Corruption in File:")
		except ValueError as e:
			log(logfile, "%s %s" % (repr(e), filename))
		if not min:
			min = 12500
		print min
		# Open with numpy load function
		# cols = numpy.arange(0,1150)		# Define number of column in data
		cols = numpy.arange(0,1264)		# Define number of column in data
		data = numpy.genfromtxt(filename,comments="#",skip_header=5,usecols=cols,filling_values=(-1*min),invalid_raise=False)
		#img[70:numpy.size(data[:,0])+70,1000:2150] = data + min
		img[:numpy.size(data[:,0]),976:] = data + min
		print "Shape of the image from 1 Mbit file: ",numpy.shape(data)

		try:
			# 1. Create header
			hdu = fits.PrimaryHDU()
			for i in range(0,len(hrd_caps)):
				hdu.header[hrd_caps[i]] = hrds[i]
				print hrd_caps[i], hdu.header[hrd_caps[i]]

			# Save output to a FITS file as "image000xxx.fits"
			# Take FITS headers (if available) from first line of txt files
			new_fits = destPath + '/image%06i.fits'%(int(hdu.header['IMNO']))
			os.rename(filename, new_fits + '.txt')

			hdu.data = img
			hdu.writeto(new_fits)
			print "Creating %s SUCCESS!" % (new_fits)
		except:
			# If writing a new file failed:
			print "Creating fits FAILED" 





	return img



if __name__ == "__main__":
	main()
