/* 
	frameblob.h

	class which takes a bitmapped frame and locates blobs (where
	blobs are stored as a linked list)

	Depends on bloblist class.

	Written by Edward Chapin      Contact: echapin@mail.sas.upenn.edu

	Last revision: September 16, 2004
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// edited by J. Zorrilla. I #include <stdint.h> to be able to use uint16_t for MAPTYPE
#include <stdint.h>

#include "bloblist.h"


#ifndef FRAMEBLOB_H
#define FRAMEBLOB_H

// edited by M. Matuszewski. We will be using Retiga's 8 bit mode
// typedef unsigned short MAPTYPE;  // map elements data type
//typedef unsigned char MAPTYPE;
// edited by J. Zorrilla. PCOEdge5.5 saves the image frame in uint16_t
typedef uint16_t MAPTYPE;


// the way the FIREBALL code runs, care must be taken when it comes to
// bookkeeping of the blobs.  

class frameblob 
{
 public:
  // --- Constructors / Destructor ---
  frameblob( void );            // Constructors
  frameblob( MAPTYPE *map_in, unsigned int xpix_in, unsigned int ypix_in,
	     unsigned int bits_in, double platescale_in );
  void commonconstructor(MAPTYPE *map_in, unsigned int xpix_in, unsigned int ypix_in,
			 unsigned int bits_in, double platescale_in );
  ~frameblob();			      // Destructor


  // --- edit frameblob parameters without creating a new objecto of
  // --- the class. Added by M. Matuszewski (June 9, '07). 

  void setXpix(unsigned int xpix_in);
  void setYpix(unsigned int ypix_in);
  void setPlatescale(double platescale_in);
  unsigned int getXpix(){ return xpix; };
  unsigned int getYpix(){ return ypix; };
  double getPlatescale(){ return platescale; };

  // update the map pointed to
  // M Matuszewski Jan 11, 2016.
  void setMap(MAPTYPE *map_in){ map = map_in; };
  
  // --- Add blob, delete blob, return blob list ---
  void addblob( int flux, double x, double y );  // Add a blob to the list
  void deleteblob( bloblist *killblob );         // Remove blob from the list
  void clearblobs();                             // Clear the whole blob list  
  bloblist *getblobs();                          // return pointer to 1st blob in list
  void sortblobs();                              // sort blobs by descending order of flux
  void set_maxblobs(unsigned int in_maxblobs);	 // set a maximum number of blobs
  
  // --- Bad pixel map ---
  int load_badpix(const char *fname); // Load bad pixel map from file
  void fix_badpix(MAPTYPE val); // set bad pixels to val
  
  // --- Set search parameters ---
  void set_satval( MAPTYPE in_satval );              // set numerical value of saturated pixels
  void set_grid( unsigned int in_grid );             // coarse grid pixsize
  void set_cenbox( unsigned int in_cenbox );         // centroid box pixsize
  void set_apbox( unsigned int in_apbox );           // aperture box size (for flux)
  void set_threshold( double in_threshold );         // n sigma source threshold
  void set_disttol( int in_disttol );                // adjacent
						     // source dist^2
						     // tolerance


  
  // --- Get search parameters ---
  MAPTYPE get_satval(void) { return satval; };
  unsigned int get_grid(void) { return grid; };
  unsigned int get_cenbox(void) { return cenbox; };
  unsigned int get_apbox(void) { return apbox; };
  double get_threshold(void) { return threshold; };
  int get_disttol(void) { return disttol; };
  
  // --- Source find / map statistics ---
  void calc_mapstat();          // calc map mean + count # saturated pixels each column
  double get_mapmean();		// return the map mean - calls mapstat first if necessary
  double get_sigma();		// return the estimates standard deviation of the image
  void calc_searchgrid();       // find cells in search grid with sources
  void calc_flux();		// sum flux in apertures, check if pointlike
  void calc_centroid();         // after search/flux, calculate centroids
  void fix_multiple();          // check for multiple sources within disttol and fix
  
  double computeFocusX();	//  calculates the average FWHM of the blobs - chonko 3/22/05
  double computeFocusY(); 

  void set_gain( double in_gain );
  void set_readout_offset( double in_readout_offset );
  void set_readout_noise( double in_readout_noise );
  
  double get_gain(void) { return gain; };
  double get_readout_offset(void) { return readout_offset; };
  double get_readout_noise(void) { return readout_noise; };

  unsigned int Numblobs(){ return numblobs; };
  
 private:
  MAPTYPE *map;			// Pointer to the map buffer
  unsigned int bits;            // number of bits per pixel
  unsigned int xpix;	        // # pixels wide
  unsigned int ypix;	        // # pixels high
  unsigned int npix;            // # pixels in total map
  
  double *searchgrid;           // coarse grid cells - store S/N brightest pixel
  double *meangrid;             // array of mean values for each grid cell
  unsigned int xgrid;           // dimensions of the search grid array
  unsigned int ygrid;           //    "
  unsigned int ngrid;           //    "
  
  double platescale;            // number of degrees/pixel in the image
  double gain;			// gain applied to N_photons going to measured values
  double readout_offset;	// constant offset in the frame (probably due to readout)
  double readout_noise;	        // constant RMS noise factor added to frame due to readout
  
  bloblist allblobs[1001];

  bloblist *firstblob; 	        // blob linked list pointers (sources)
  bloblist *lastblob;
  bloblist *thisblob;           // multi-purpose blob pointer for traversing the list
  unsigned int numblobs;
  unsigned int maxblobs;        // when finding blobs won't exceed this number if non-zero
  
  unsigned int *badpixels;      // array of indices of bad pixels in CCD frames
  unsigned int numbadpix;       // # of bad pixels
  
  double mapmean;               // the mean value of the total map
  double sigma;			// estimated standard deviation of the map
  MAPTYPE satval;               // numerical value of a saturated pixel
  int *satcol;                  // array contains # saturated pixels in each column of the map
  
  unsigned int grid;	        // # pixels/side in coarse grid
  unsigned int apbox;           // # pixels/side aperture photometry box
  unsigned int cenbox;          // # pixels/side centroiding box
  double threshold;             // N sigma threshold for detection	
  int disttol;                  // distance tolerance for adjacent sources in pixels

  //  MAPTYPE *cell;
};

#endif //FRAMEBLOB_H
