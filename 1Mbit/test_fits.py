import os
import subprocess
import sys
import tarfile
import time
import numpy
from datetime import datetime, timedelta
from astropy.io import fits

def main():
        move_file('image_0.txt', '/home/fireball2/dobc/data/230920', 'filetrack_rx_230903_1800')

def move_file(file,destPath,logfile):
	'''
	Move the file from where it gets transmitted to to appropriate folder.
	'''
	print(file)
	os.rename(file,os.path.join(destPath,file))
	print(file)
	# Try to write to fits file
	im = write_fits(os.path.join(destPath,file), destPath, logfile)
	returncode = 0

	return returncode
        
def write_fits(filename, destPath, logfile):
	# Open fits with astropy
	print(destPath)
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
								'TEMPD',
							]
		# Try to read in min value from file
		#try:
		lines = open(filename).readlines()
		fields_headers = lines[0].split()
		hrds = fields_headers[3:]
		print "test output"
		print fields_headers
		print hrds
		fields1 = lines[2].split()
		fields2 = lines[3].split()
		fields3 = lines[4].split()
		print lines[2], fields2, fields3
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
		#except ValueError as e:
			#log(logfile, "%s %s" % (repr(e), filename))
		if not min:
			min = 12500
		print min
		# Open with numpy load function
		cols = numpy.arange(0,1264)		# Define number of column in data
		data = numpy.genfromtxt(filename,comments="#",skip_header=5,usecols=cols,filling_values=(-1*min),invalid_raise=False)
		img[:numpy.size(data[:,0]),976:] = data + min
		print "Shape of the image from 1 Mbit file: ",numpy.shape(data)
                print img 


		try:
			# 1. Create header
			hdu = fits.PrimaryHDU()
			for i in range(0,len(hrd_caps)):
				hdu.header[hrd_caps[i]] = hrds[i]
				print hrd_caps[i], hdu.header[hrd_caps[i]]

			# Save output to a FITS file as "image000xxx.fits"
			# Take FITS headers (if available) from first line of txt files
			new_fits = destPath + '/image%06i.fits'%(int(hdu.header['IMNO']))
			print(new_fits)
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

