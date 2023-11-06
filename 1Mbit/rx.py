# MUST be run through virtualenv!:
#
# voldy:/home/fireball2/Communications/comsync-r17-port1/python_env/bin/python
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
### Changes on 2023-08-21:
#               - Change the file path to python_env folder
#               - Change to comsync_r17-port1

import os
import subprocess
import sys
import tarfile
import time
from datetime import datetime, timedelta
#~ from astropy.io import fits


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

	# Initialize log file
	timeinit = datetime.now()
	logfile = "filetrack_rx_%02i%02i%02i_%02i%02i.log" % (
		timeinit.year % 100,
		timeinit.month,
		timeinit.day,
		timeinit.hour,
		timeinit.minute
	)

	# Hard-code one filename for Rx to write to every time
	filename = "rxdata.tar.gz"
	#program = ". /home/fireball2/Communications/comsync-r17-port2/test"		# Executable from test.c
        program = "./test"
	#~ filedir = "/home/fireball2/Communications/comsync-r15-port1/rxdata/"
	#~ tardir = "/home/fireball2/Communications/comsync-r15-port1/rxtar/"

	# Test
	filedir = "/home/fireball2/1Mbit/rxdata/"
	tardir = "/home/fireball2/1Mbit/rxtar/"
	# Real-time directories
	#~ filedir = "/home/fireball2/1Mbit/data/"+datetime.now().strftime("%y%m%d")+"/rxdata/"
	#~ tardir = "/home/fireball2/1Mbit/data/"+datetime.now().strftime("%y%m%d")+"/rxtar/"
	num = 130
	packet = 256
	delay = 5 # Seconds to wait before moving on to next file
	final_tar = "temp.tar"

	# determine if directories exist
	# if not, create them
	if not os.path.exists(filedir):
		print filedir+" does not exist: creating..."
		try:
			os.makedirs(filedir)
			print filedir+" created!"
		except:
			print filedir+" FAILED - check permissions"
	else:
		print filedir+" exists already."

	if not os.path.exists(tardir):
		print tardir+" does not exist: creating..."
		try:
			os.makedirs(tardir)
			print tardir+" created!"
		except:
			print tardir+" FAILED - check permissions"
	else:
		print tardir+" exists already."




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

		# Receive tar file using data transfer C code
		try:
			# 1. Rx Purge
			# Try to purge RX after file transfer complete
			
			try:
				p = subprocess.Popen([program],
														 stdin=subprocess.PIPE,
														 stdout=subprocess.PIPE,
														 bufsize=1)
				pid = p.pid
                                print(pid)
                                #p.stdin.write("y\n y\n y\n")
				p.stdin.write("y\n y\n y\n 7\n")
				time.sleep(delay)		# Wait for purge to pass and not block up receiver?
				print "Rx purge complete"
				log(logfile,"Rx purge: SUCCESS")
			except:
				print "Rx purge didn't work?"
				log(logfile,"Rx purge: FAILED")


			# 2. Receive data
			timeout = 3		# Set time for delay to check change in file size to exit receiver
			log(logfile,"Running 1 Mbit transfer ...")
			print "Running 1 Mbit transfer..."
			# Tell ./test to receive a file
			p.stdin.write("12\n %i\n 1\n 2\n %s\n" % (packet,filename))
			# Use this to break from receiver (TRY)
			oldsize = 0
			while True:
				#print out
				time.sleep(timeout)
				size = os.path.getsize(filename)
				print size
				if size == 0:
					continue
				elif size == oldsize:
					print "file transfer complete"
					log(logfile,"file transfer complete: file size = %i bytes"%(size))
					os.kill(pid, 9)
					break
				oldsize = size
				#returncode = 0

			# move tar file to tar directory
			try:
				newtar = "rxdata_%i.tar.gz"%(num)
				subprocess.call("cp "+filename+" "+tardir+newtar, shell=True)
				num += 1
				print "Moving tar to new directory SUCCESSFUL!"
				log(logfile,"Moving Rx tar to new directory: SUCCESSFUL!")
			except:
				print "Moving tar to new directory FAILED :("
				log(logfile,"Moving Rx tar to new directory: FAILED")

			# Untar file
			try:
				#~ subprocess.check_call("tar -xvzf "+filename+" --strip-components=4 -C "+filedir, shell=True)
				tar = tarfile.open(filename,'r:gz')
				tar.extractall()
				tar.close()
				tar = tarfile.open(final_tar,'r')
				tar.extractall(path=filedir)
				tar.close()
				print "Untarring Rx file SUCCESSFUL! Onto next file..."
				log(logfile,"Untarring Rx tar to new directory: SUCCESSFUL!")
			except:
				# May have noisy byte(s) at beginning of file, so try stripping 1, 2, 3
				# before final failure mode
				print "Untarring Rx FAILED :( --> Try stripping 1 byte"
				log(logfile,"Untarring Rx tar to new directory: FAILED --> Try stripping 1 byte")
				# 1 byte
				try:
					# Linux command to strip bytes
					stripfile = "rxdata_strip.tar.gz"
					subprocess.call("dd if="+filename+" of="+stripfile+" ibs=1 skip=1", shell=True)
					#~ subprocess.check_call("tar -xvzf "+stripfile+" --strip-components=4 -C "+filedir, shell=True)
					tar = tarfile.open(stripfile,'r:gz')
					tar.extractall()
					tar.close()
					tar = tarfile.open(final_tar,'r')
					tar.extractall(path=filedir)
					tar.close()
					print "Untarring 1 byte-stripped Rx file SUCCESSFUL! Onto next file..."
					log(logfile,"Untarring 1 byte-stripped Rx tar to new directory: SUCCESSFUL!")
				except:
					print "Stripping 1 byte + Untarring Rx FAILED :( --> Try stripping 2 bytes"
					log(logfile,"Stripping 1 byte + Untarring Rx tar to new directory: FAILED --> Try stripping 2 bytes")
					# 2 bytes
					try:
						# Linux command to strip bytes
						stripfile = "rxdata_strip.tar.gz"
						subprocess.call("dd if="+filename+" of="+stripfile+" ibs=2 skip=1", shell=True)
						#~ subprocess.check_call("tar -xvzf "+stripfile+" --strip-components=4 -C "+filedir, shell=True)
						tar = tarfile.open(stripfile,'r:gz')
						tar.extractall()
						tar.close()
						tar = tarfile.open(final_tar,'r')
						tar.extractall(path=filedir)
						tar.close()
						print "Untarring 2 byte-stripped Rx file SUCCESSFUL! Onto next file..."
						log(logfile,"Untarring 2 byte-stripped Rx tar to new directory: SUCCESSFUL!")
					except:
						print "Stripping 2 bytes + Untarring Rx FAILED :( --> Try stripping 3 bytes"
						log(logfile,"Stripping 2 bytes + Untarring Rx tar to new directory: FAILED --> Try stripping 3 bytes")
						# 3 bytes
						try:
							# Linux command to strip bytes
							stripfile = "rxdata_strip.tar.gz"
							subprocess.call("dd if="+filename+" of="+stripfile+" ibs=3 skip=1", shell=True)
							#~ subprocess.check_call("tar -xvzf "+stripfile+" --strip-components=4 -C "+filedir, shell=True)
							tar = tarfile.open(stripfile,'r:gz')
							tar.extractall()
							tar.close()
							tar = tarfile.open(final_tar,'r')
							tar.extractall(path=filedir)
							tar.close()
							print "Untarring 3 byte-stripped Rx file SUCCESSFUL! Onto next file..."
							log(logfile,"Untarring 3 byte-stripped Rx tar to new directory: SUCCESSFUL!")
						except:
							print "Stripping 3 bytes + Untarring Rx FAILED :( --> Continuing..."
							log(logfile,"Stripping 3 bytes + Untarring Rx tar to new directory: FAILED --> Continuing...")

		except:
			print "Unable to open program --- ./test likely still open!!!!!"
			log(logfile,"UNABLE TO OPEN RX LINE!: ./test likely still open!!!!!")
			os.kill(pid, 9) # 9 or 15
			returncode = 1






# Wrap print+file output together
def log(logfile, string):
	print(string)
	with open(logfile, 'a') as f:
		f.write(string + '\n')








if __name__ == "__main__":
	main()
