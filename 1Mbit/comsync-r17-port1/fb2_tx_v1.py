# MUST be run through virtualenv!:
#
# snape:/home/fireball2/Documents/Communications/comsync-r15/python_env/bin/python
# (or in the folder: python_env/bin/python )
#
#
#
# Handles the compression of new image and houskeeping files
# on the DOBC during flight:
#			- Search for new image file (incremented by number)
#			- Compress new image file and existing housekeeping files (separately)
#			- Send each compressed file through the 1 MBit transmitter/receiver c-code
#			- increment +1 to image file count.
#
##
### MAJOR CHANGES:
##
#		- Files no longer compressed - send down as-is
#		- FITS files --> Converted to TXT files (including header info)
#

import os
import subprocess
import sys
import tarfile
import time
from datetime import datetime, timedelta
from astropy.io import fits
import numpy
#~ import shutil


def main():
	if os.name == "posix":
		root = "/home/fireball2"
		dir = '/'
	elif os.name == "nt": # windows
		root = "C:\\Users\\Keri Hoadley\\Documents\\FIREBall-2\\python"
		dir = '\\'
	else:
		print "Unrecognized platform, exiting."
		exit(1)

	directories = {
		"hk": "data/",
		"img": "data/",
		#~ "img": ""
	}

	for key, value in directories.iteritems():
		directories[key] = os.path.normpath(os.path.join(root, value))+dir

	filename_root = "image" # Substring to find in filenames
	#### INCORPORATES OPTIONAL ARG at end of file call: python fb2_tx.py [number]
	#### If number is called, will start from there
	#### In no number called, will start at 0
	if len(sys.argv) == 2:
		print "Arguments called: ", sys.argv[1]
		filenum0 = int(sys.argv[1])
	else:
		filenum0 = 0			# IMAGE File number you want to start on
	print "Initial starting number: ", filenum0
	#~ filenum0 = 0 # File number you want to start on

	# Housekeeping names not expected to change
	housekeeping = [
		"alltemps.csv",
		"cooler_power.csv",
		#"discretecom.csv",
		#"waterpress.csv",
	]

	# Various delay times. Need to set properly!!!
	delay = 10                      # Seconds to wait before moving on to next file
	delay_image = 95               # Seconds to wait for one image to transfer successfully
	delay_hk = 10                   # Seconds to wait for housekeeping files (csv) to transfer successfully
	program = "./test"		# Executable from test.c

	# Other variables
	timeinit = datetime.now()
	logfile = "filetrack_tx_%02i%02i%02i_%02i%02i.log" % (
		timeinit.year % 100,
		timeinit.month,
		timeinit.day,
		timeinit.hour,
		timeinit.minute
	)
	filenum = filenum0

	# Open the ./test.c code so that the BitSync is locked all the time. The code will stay running in the background.
	# Will need to kill with PID
	try:
		p = subprocess.Popen([program],stdin=subprocess.PIPE,stdout=subprocess.PIPE,bufsize=1)
		pid = p.pid
                print(pid)
		p.stdin.write("y\n y\n y\n")
				
		print "Opened ./test"
		log(logfile,"Open ./test: SUCCESS")
	except:
		print "Could not open ./test"
		log(logfile,"Open ./test: FAILED")
	time.sleep(delay)		# Delay so that the Receiver side code is opened properly




	#Keep running program unless something sets break condition
	while True:
		# Log the start time
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

		# Get filename for this image
		filename = "%s%06i.fits" % (filename_root, filenum)
		filepath = os.path.join(directories["img"]+datetime.now().strftime("%y%m%d"), filename)
		print filepath

		log(logfile, "File: %s" % filename)

		# Keep track of whether each file is found
		filefound = os.path.isfile(filepath)

		# If file exists:
		if filefound:
			# 1a. Create a copy of image.fits in backup drives
			returncode = backup_fits(directories["img"]+datetime.now().strftime("%y%m%d"), filename)
			if returncode != 0:
				log(logfile,"Copy of %s FAILED --> continuing..." % (filename))
			else:
				log(logfile,"%s Copied to USB Drives!" % (filename))

			# 1. Read in FITS image, same header and 2D image array to a txt file.
			rtncode, new_filepath = read_fits(filepath, logfile)
			if rtncode != 0:
				log(logfile, "ERROR: Converting FITS to TXT for %s FAILED" % (filepath))
				continue
			else:
				log(logfile, "--> Sending %s to comm board" % (new_filepath))

			# 2. Send image file through transmitter
                        try:
                        	p.stdin.write("11\n 256\n 1\n 2\n %s\n y\n" % (new_filepath))
                                log(logfile, "Sending %s!" % (new_filepath))
			except:
				log(logfile, "Error executing command: %s %s" % (program, new_filepath))

			# Now, delay for some time so that the image is sent properly
			time.sleep(delay_image)
			filenum += 1


			# 2. Housekeeping tarball send through transmitter
			for hk in housekeeping:
				hkfile = os.path.join(directories["hk"]+datetime.now().strftime("%y%m%d"),hk)
				log(logfile, "--> Sending %s to comm board" % (hkfile))
				try:
                        		p.stdin.write("11\n 256\n 1\n 2\n %s\n y\n" % (hkfile))
                                        log(logfile, "Sending %s!" % (hkfile))
			        except:
					log(logfile, "Error executing command: %s %s" % (program, hkfile))


				# Again, delay a short time so that the hk files are sent properly
				time.sleep(delay_hk)

			log(logfile, "Next image number looking for: %06i" % (filenum))
			time.sleep(delay)		# Add delay to allow for Rx PURGE time

		#If no file or tarball exist, probably just not created yet. Just wait.
		elif not filefound:
			# Wait the given amount of time before moving on
			log(logfile, "FITS file not created/found yet.")
			log(logfile, "Sleeping... %2i seconds" % (delay))

			# Still send down
			time.sleep(delay)

		# If tarball exists but no file. This is always an error.
		else:
			log(logfile, "SOMETHING IS WRONG -- BREAKING." % filepath)
			break





# Wrap print+file output together
def log(logfile, string):
	print(string)
	with open(logfile, 'a') as f:
		f.write(string + '\n')





def read_fits(filename,logfile):
	new_file = filename+".txt"	# New file extension
	ymin = 70
	ymax = 2000
	xmin = 1000
	xmax = 2150

	try:
		# Open fits with astropy
		hdulist = fits.open(filename)
		print "Opened image file:"
		# CHANGE TO CORRECT CALLABLE ONCE KNOWN
		hrd = hdulist[0].header		# Can do hdr.comments[' '] to check header comments
		data = hdulist[0].data
		data = data[ymin:ymax,xmin:xmax]
		mindata = numpy.min(data)
		data = data - mindata
		data = numpy.int_(data)
		print "Defined image area to transmit:"
		h1 = hrd['DATE'] # %s - YYYY-MM-DDThh:mm:ss UT <--
		#h2 = hrd['IMG'] # %s  <--
		h3 = hrd['EXPTIME'] # %03i  <--
		h4 = hrd['EMGAIN'] # %04i <--
		#h5 = hrd['PREAMP'] # %.2f  <--
		#~ h6 = hrd['IMNO'] # %03i
		#~ h7 = hrd['IMBURST'] # %01i
		#~ h8 = hrd['SHUTTER'] # %01i
		h9 = hrd['TEMPD'] # %s  <--
		hdulist.close()

		# Write the array to disk
		with file(new_file, 'w') as outfile:
			outfile.write('# Header info: %s \t %03i \t %04i \t %s \n' % (h1,h3,h4,h9))
			outfile.write('# MIN: \n' )
			outfile.write('# %05i \n' % (mindata))
			outfile.write('# \t %05i \n' % (mindata))
			outfile.write('# \t \t %05i \n' % (mindata))
			numpy.savetxt(outfile, data, fmt='%i')#, fmt='%.4f')
		returncode = 0
	except:
		returncode = 1

	return returncode, new_file



def backup_fits(origPath, filename):
	'''
	Use to back up image fits files to USB drives in:
	/home/fireball2/databackup0
	/home/fireball2/databackup1
	'''
	origFile = os.path.join(origPath, filename)
	destPath1 = "/home/fireball2/databackup0"
	destPath2 = "/home/fireball2/databackup1"
	if os.name == "posix":		# linux: run ./test for transmission
		try:		# Move to USB 1, then 2
			newfile1 = os.path.join(destPath1, filename)
			subprocess.Popen(["cp", "-v", origFile, newfile1])
			newfile2 = os.path.join(destPath2, filename)
			subprocess.Popen(["cp", "-v", origFile, newfile2])
			returncode = 0
		except:
			print "Copy to USB Drive FAILED - Continue"
			returncode = 1
	else:
		print "Not Linux -- continue"
		returncode = 1

	return returncode



if __name__ == "__main__":
	main()
