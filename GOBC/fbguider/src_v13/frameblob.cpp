/* 
frameblob.h

class which takes a bitmapped frame and locates blobs (where
blobs are stored as a linked list)

Depends on bloblist class.

Written by Edward Chapin      Contact: echapin@mail.sas.upenn.edu

Last revision: September 16, 2004
*/

#include "frameblob.h"

#include <stdio.h>
#include <iostream>
#include <string.h>
using namespace std;



// ---------- Class constructor -----------------------------------------------

frameblob::frameblob( void )
{
}

frameblob::frameblob( MAPTYPE *map_in, unsigned int xpix_in, unsigned int ypix_in,
					 unsigned int bits_in, double platescale_in )
{
	commonconstructor( map_in, xpix_in, ypix_in, bits_in, platescale_in );

}

void frameblob::commonconstructor( MAPTYPE *map_in, unsigned int xpix_in, 
				   unsigned int ypix_in, unsigned int bits_in, double platescale_in )
{
	// set up map parameters
	map = map_in;
	xpix = xpix_in;
	ypix = ypix_in;
	bits = bits_in;
	platescale = platescale_in;
	npix = xpix*ypix;

	// Blob list pointers initialized to 0
	numblobs = 0;
	maxblobs = 0;

	firstblob = NULL;
	lastblob = NULL;
	thisblob = NULL;

	// Set the gridsearch array to NULL so we can check at the end if allocated
	searchgrid = NULL;
	meangrid = NULL;

	// Map statistics defaults
	mapmean=0;
	sigma=1;
	// map statistics defaults edited by Jose Zorrilla
	satval = 65535;
	satcol = new int[xpix_in];
	//satval = 255;               // numerical value of a saturated pixel. Edited to 255 by M. Matuszewski
	//satcol = new int[1360];       // array contains # saturated pixels in each column of the map

	gain=1;
	readout_offset=0;
	readout_noise=0;

	// Defaults for search parameters
	grid = 32;         // pixels/side in coarse grid search
	cenbox = 17;       // pixels/side in centroid box
	apbox = 5;         // pixels/side in aperture photometry box
	threshold = 5.0;     // N sigma threshold criterion
	disttol = 30*30;   // distance^2 tolerance for adjacent sources

	badpixels = NULL;

	//	searchgrid = new double[40000];
	//	meangrid = new double[40000];
      
	//	printf("lalala\n");
	///	cell = new MAPTYPE[40000];
	//	printf("lalala2\n");
	//printf("%i %i\n",xpix,ypix);
}

// ---------- Class Destructor ------------------------------------------------

frameblob::~frameblob()
{

	clearblobs();

	delete[] searchgrid;
	delete[] meangrid;
	delete[] badpixels;
	delete[] satcol;
	//	delete[] cell;

}

// ---------- Add blob to list ------------------------------------------------

void frameblob::addblob( int flux, double x, double y )
{
	// Only add a new blob if we haven't exceeded the maximum number, or if
	// we can have an unlimited number (maxblob=0)

	//printf("nblobs: %i maxblobs: %i\n",numblobs,maxblobs);

	if( (maxblobs == 0) || (numblobs < maxblobs) )
	{
		thisblob = new bloblist(flux, x, y);

		if( lastblob == NULL )  // make new starting blob if empty list
		{
			firstblob = thisblob;
			lastblob = thisblob;		
			numblobs = 1;
		}
		else                    // otherwise add to the end
		{
			lastblob->setnextblob( thisblob );
			thisblob->setprevblob( lastblob );
			numblobs++;
		}

		lastblob = thisblob;    // new end of list
	}
}

// ---------- Delete blob from list ------------------------------------------------

void frameblob::deleteblob( bloblist *killblob )
{
  bloblist *prev, *next;

  prev = killblob->getprevblob();
  next = killblob->getnextblob();

  // Set the head/tail pointers for the surrounding blobs to skip over this one
  
  if( prev == NULL ) {      // if 1st blob in list
    
    if( next == NULL )    // if was only blob in list
		{
		  firstblob = NULL;
		  lastblob = NULL;
		}
    else                  // otherwise next is new start
      {
	firstblob = next;
	next->setprevblob(NULL);
      }
  }
  else
    {
      if( next == NULL )    // was last element in multi-blob list
	{
	  prev->setnextblob(NULL);
	  lastblob = prev;
	}
      else                  // somewhere in the middle of a list
		{
		  prev->setnextblob(next);
		  next->setprevblob(prev);
		}
    }
  
  // Delete this blob
  
  delete killblob;
  
  // Decrement the blob counter
  
  numblobs --;
}

// ---------- Clear whole blob list ------------------------------------------------

void frameblob::clearblobs()
{
  bloblist *nextblob;
  thisblob = firstblob;  // start at the beginning of the list
  while( thisblob != NULL )
    {
      nextblob = thisblob->getnextblob(); // get next blob
      delete thisblob;                    // delete the current one
      thisblob = nextblob;                // move along
    }
  
  firstblob=NULL;
  lastblob=NULL;
  thisblob=NULL;
  numblobs = 0;
}

// ---------- Return start of blob list -------------------------------------------

bloblist *frameblob::getblobs()  // pointer to first blob
{
	return firstblob;
}

// ---------- Sort the blob list in descending flux order -------------------------

void frameblob::sortblobs()
{
   bloblist *nextblob, *startblob, *endblob;
  int flag=1;   // if set, list is still out of order
  
  if( numblobs > 0 )
	{
	  while( flag == 1 )
	    {
	      flag=0;
	      thisblob = firstblob;
	      nextblob = thisblob->getnextblob();
	      
	      while( nextblob != NULL )   // Go through the list, swap if out of order
		{
		  if( nextblob->getflux() > thisblob->getflux() )
		    {   
		      flag=1;    // list still out of order
		      
		      startblob = thisblob->getprevblob();
		      endblob = nextblob->getnextblob();
		      
		      // Change the forward pointers
		      if( startblob != NULL ) startblob->setnextblob(nextblob);
					else firstblob = nextblob;   // since the start of the list has changed
		      nextblob->setnextblob(thisblob);
		      thisblob->setnextblob(endblob);
		      
		      // Change the backward pointers
		      if( endblob != NULL ) endblob->setprevblob(thisblob);
		      else lastblob = thisblob;
		      thisblob->setprevblob(nextblob);
		      nextblob->setprevblob(startblob);
		      
		      nextblob = endblob;
		    }
		  else thisblob = nextblob;
		  
		  nextblob = thisblob->getnextblob();
		}
	    }
	}
}

// set a maximum number of blobs
void frameblob::set_maxblobs(unsigned int in_maxblobs)
{
  maxblobs = in_maxblobs;
}

// ---------- Deal with bad (extra noisey) pixels ---------------------------------

// Load bad pixel map from file. returns -1 on error, 1 for success

int frameblob::load_badpix(const char *fname)
{
   FILE *badfile;
   bloblist *firstbad, *thisbad, *nextbad, *lastbad;  // bad pixel blob list
   char thisline[81];
   int count;     // Number of elements read from the current line
   int i, x, y;   // coordinates of the bad pixel in this line
   
  if( (badfile = fopen( fname, "r" )) == NULL )
    return -1;
  
  // Read through file line-by-line reading in pixel coordinates.
  // Use the blob linked-list to dynamically add coordinates as we
  // read them in. At the end, put the array indices calculated from the
  // pixel coordinates into an array for quick access when we process
  // frames.
  
  numbadpix = 0;   
  firstbad = NULL; 
  
  while( fgets(thisline,80,badfile) != NULL )
    {
      count = sscanf(thisline,"%i %i",&x, &y);
      
      if( count == 2 )  // If we read in a coordinate pair
	{
	  numbadpix ++;
	  thisbad = new bloblist(0, (double)x, (double)y);
	  
	  if( firstbad == NULL )  // If first bad pixel in the list
	    {
	      firstbad = thisbad;
	    }
	  else                    // adding on new bad pixel to list
	    {
	      lastbad->setnextblob(thisbad);
	      thisbad->setprevblob(lastbad);
	    }
	  
	  lastbad = thisbad;
	}
    }
  fclose(badfile);
  
  // Now traverse (and delete) linked list, calculating pixel coordinates
  
  badpixels = new unsigned int[numbadpix];
  thisbad = firstbad;  // start at the beginning of the list
  
  i = 0;
  
  //printf("badpixels: %i\n",numbadpix);
  while( thisbad != NULL )
    {
      badpixels[i] = (unsigned int) thisbad->gety()*xpix + 
	(unsigned int) thisbad->getx();
      
      i++;
      nextbad = thisbad->getnextblob();  // get next blob
      delete thisbad;                    // delete the current one
      thisbad = nextbad;                 // move along
    }
  
  return 1;
}


// Set bad pixels to a value

void frameblob::fix_badpix(MAPTYPE val)
{
   int i;
  
  if( badpixels != NULL )
    for( i=0; i<(int)numbadpix; i++ )
      {
	map[badpixels[i]] = val;
      }
}

// ---------- Search Parameters --------------------------------------------------

void frameblob::set_satval( MAPTYPE in_satval )
{
  satval = in_satval;
}

void frameblob::set_grid( unsigned int in_grid )
{
  grid = in_grid;
}

void frameblob::set_cenbox( unsigned int in_cenbox )
{
  cenbox = in_cenbox;
}

void frameblob::set_apbox( unsigned int in_apbox )
{
  apbox = in_apbox;
}

void frameblob::set_threshold( double in_threshold )
{
  threshold = in_threshold;
  
  if( threshold < 0.2 ) threshold = 0.2;  // place a hard limit
}

void frameblob::set_disttol( int in_disttol ) 
{
  disttol = in_disttol;
}

// ---------- Calculate map statistics: mean and # saturated pixels/column ------------------------

void frameblob::calc_mapstat()
{
  unsigned int i, j, count,index;
  
  // initialize variable
  
  mapmean = 0;
  count = 0;

  memset(satcol,0,xpix*sizeof(int));
  
  for( j=0; j<ypix; j++ )        // j=row
    {
      index = j*xpix;            // inner loop over columns to speed up addressing slightly
      for( i=0; i<xpix; i++ )
	{
	  //printf("%i ",map[index]);x	
	  if( map[index] >= satval ) satcol[i]++;   // if saturates pixels, add to column totals
	  else                                      // otherwise contributes to mean
	    {
	      if( map[index] != satval ) // pixel isn't bad
		{
		  mapmean += map[index];
		  count ++;
		}
	    }
	  index++;
	}
    }
//
  
  if( count > 0 ) mapmean = mapmean / (double) count;   // prevent divide by zero
  if( mapmean < 1 ) mapmean = 1;			// prevent 0 map means
 
}

double frameblob::get_mapmean()
{
  if( mapmean == 0 ) calc_mapstat();
  return mapmean;
}

double frameblob::get_sigma()
{
  return sigma;
}


// ---------- Search grid for coarse cells with sources ----------------------------

void frameblob::calc_searchgrid()
{
  // First calculate dimensions of the searchgrid array
   unsigned int i, j, k, l, mapindex, cellindex;
   //i = j = k = l = mapindex = cellindex = 0;
  unsigned int xfull, yfull, xextra, yextra, xdim, ydim;
  //  xfull = yfull = xextra = xdim = ydim = 0;
  
  xfull  = xpix / grid;       // # of full-sized cells
  yfull  = ypix / grid;	
  xextra = xpix % grid;       // # extra pixels in small edge cells
  yextra = ypix % grid;
  
  xgrid = (xfull + (xextra>0));        // total # grid cells 
  ygrid = (yfull + (yextra>0));
  ngrid = (xgrid+xextra) * ygrid;
  
  // unallocate searchgrid and meangrid before we start in case we have run this 
  // function several times 

  // we will zero instead of unallocating. this should be faster.

  /*
  for(i=0;i<ngrid;i++){
    searchgrid[i] = 0.0;
    meangrid[i]=0.0;
  };

  for(i=0;i<grid;i++)
    for(j=0;j<grid;j++)
      cell[i+j*grid]=0.0;
  */
    
  delete searchgrid;
  delete meangrid;
  
  searchgrid = new double[ngrid];   // S/N of brightest pixel/grid cell
  meangrid = new double[ngrid];     // array of grid cell means 
    
  // Allocate an array to contain all the GRID*GRID elements within this
  // grid cell of our input map
  
  //  commented out as it is now part of the constructor.
      MAPTYPE *cell;
    cell = new MAPTYPE[grid*grid];
  
  
  MAPTYPE pix;		 // current pixel values
  MAPTYPE max;		 // the maximum value pixel in the cell
  int total;             // running total of elements in cell (big number)

  double meancell;       // cell mean
  //double sigma;        // sample standard deviation
  double level;	         // max adjusted by centre value
  double sn;		 // signal to noise level of brightest pixel
  double x=0;		 // x and y pixel positions for brightest pixels
  double y=0;		 //   for the large input map  
  
  // Loop over each cell in our coarse grid
  

  
	xdim = grid;     // Pixel dimensions of grid cell. grix*grid unless edge
	ydim = grid;
	
	clearblobs();    // Make sure we don't have old blobs sitting around
	
	for( i=0; i<xgrid; i++ )            
	  for( j=0; j<ygrid; j++ )        
	    {  
	      // Sum all the elements within this cell of the input map.
	      // Also make a list of the elements in the cell
	      
	      cellindex = 0;
	      total = 0;
	      max = 0;
	      
	      if( i==xfull ) xdim = xextra;
	      else xdim = grid;		//  added 2/7/04  bugfix - chonko
	      if( j==yfull ) ydim = yextra;
	      else ydim = grid;		//  added 2/7/04  bugfix - chonko
	      
	      for( k=0; k<xdim; k++ )     
		for( l=0; l<ydim; l++ ) 
		  {
		    mapindex = (j*grid+l)*xpix + (i*grid+k);
		    pix = map[mapindex];
		    
		    cell[cellindex] = pix;
		    total += pix;
		    
		    // Check for greatest pixel brightness
		    if( pix > max ) 
		      {
			x = (double) (i*grid + k);    // store pixel coordinates
			y = (double) (j*grid + l);	// of brightest pixel					
			max = pix; 
		      }					
		    cellindex ++;   // end of loop = # pixels in cell		
		  }
	      
	      // Get the mean for this cell
	      meancell = (double) total / (cellindex);
	      
	      // Level is the brightness of a pixel - the mean level for the cell
	      level = (double) max - meancell; 
	      
	      // Calculate the sample variance about the central value
	      if( mapmean == 0 ) calc_mapstat();      // Calculate the map mean if not previously done
	      
	      if( mapmean < readout_offset ) sigma = readout_noise;
	      else sigma = (double)sqrt(gain*(mapmean-readout_offset) + readout_noise*readout_noise);   
	      // above line assumes Poission statistics
	      
	      if( sigma < 1 ) sigma = 1;   // prevent 0 sigmas
	      
	      sn = level/sigma;
	      
	      // Store information about this grid cell
	      searchgrid[j*xgrid+i] = sn;     // s/n ratio of brightest pixel in this cell
	      meangrid[j*xgrid+i] = meancell; // mean of this cell
	      
	      // If we got a pixel > threshold sigma, probably a source, 
	      if( sn >= threshold )
		{
		  addblob( (int)level, x, y );        // add blob to linked list
		  thisblob->setmean(meancell);
		  //thisblob->setsig(sigma);
		}
	    }
	
	// clean up
	
	delete cell;
	
}

// ---------- Calculate centroids centered over bright pixel locations -----------

void frameblob::calc_centroid()
{
  // Traverse the blob linked list to get rough blob locations, and perform
  // centroid operations over those positions.
  // Also calculate a new baseline estimate from the perimeter pixels of the
  // centroid box
  
  thisblob = firstblob;
  
  int startx, starty, endx, endy, i, j;//, ncen;
  double xcen, ycen, thisflux, totalflux;
  
  double mean_perimeter;
  
  while( thisblob != NULL )
    {
      startx = (int) thisblob->getx() - cenbox/2;  // centroiding box boundary
      starty = (int) thisblob->gety() - cenbox/2;
      endx = startx + cenbox-1;
      endy = starty + cenbox-1;

      if( startx < 0 ) startx = 0;                 // clipping for boundaries
      if( endx >= (int) xpix ) endx = xpix-1;
      if( starty < 0 ) starty = 0;
      if( endy >= (int) ypix ) endy = ypix-1;
      
      //ncen = (endx-startx+1)*(endy-starty+1); // # pixels in centroid box
      
      // Calculate the centroid value for the search box
      
      xcen = 0;    // Initialize centroid values to 0
      ycen = 0;
      totalflux = 0;
      
      mean_perimeter = 0;

      for( i=startx; i<=endx; i++ )
	{
	  for( j=starty; j<=endy; j++ )
	    {
	      thisflux = map[j*xpix + i];
				totalflux += thisflux;
				xcen += thisflux * (double)i;
				ycen += thisflux * (double)j;
				
				if( (i==startx) || (i==endx) || (j==starty) || (j==endy) )
					mean_perimeter += thisflux;
	    }
	}
      
      xcen /= totalflux;
      ycen /= totalflux;
      
      xcen += 0.5;
      ycen += 0.5;
      
      // Update the blob with the new centroid coordinates
      
      thisblob->setx(xcen);
      thisblob->sety(ycen);
      
      // Update the blob baseline estimate
      
      mean_perimeter = mean_perimeter/( 2*(endx-startx) + 2*(endy-starty) );
      thisblob->setmean(mean_perimeter);
      
      // Move to the next blob in the list
      
      thisblob = thisblob->getnextblob();
	}
  
  thisblob = lastblob;
}

// ---------- Calculate flux in an aperture centered over bright pixel -----

void frameblob::calc_flux()
{
  // Traverse the blob linked list, calculate fluxes in apbox*apbox apertures.
  // If the integrated flux excluding the brightest centre pixel is significant,
	// record the integrated flux, and flag as extended.
  
  thisblob = firstblob;
  bloblist *nextblob;
  
  int startx, starty, endx, endy, i, j;
  double thisflux, nap, centreflux;
  
  while( thisblob != NULL )
    {
      nextblob = thisblob->getnextblob();         // pointer to next blob
      
      startx = (int) thisblob->getx() - apbox/2;  // Aperture boundaries
      starty = (int) thisblob->gety() - apbox/2;
      endx = startx + apbox-1;
      endy = starty + apbox-1;
      
      if( startx < 0 ) startx = 0;          // clipping for map boundaries
      if( endx >= (int) xpix ) endx = xpix-1;
      if( starty < 0 ) starty = 0;
      if( endy >= (int) ypix ) endy = ypix-1;
      
      thisflux = 0.;
      
      nap = (double) (endx-startx+1)*(endy-starty+1); // # pixels in the apertures
      
      // add up flux around centre pixel
      
      for( i=startx; i<=endx; i++ )
	for( j=starty; j<=endy; j++ )
	  {
	    thisflux += (double) map[j*xpix + i];
	  }
      
      // remove the baseline
      
      thisflux = thisflux - (nap*thisblob->getmean());
      
      // remove flux of centre pixel and see if remaining flux significantly
      // exceeds the theoretical flux from pure noise (check extendedness)
      
      centreflux = (double) thisblob->getflux();
      thisflux = thisflux - centreflux;
      
      // Extended case
      if( (thisflux/(sqrt(nap-1)*sigma)) >= threshold )
	{
	  // Change the blob flux to the integrated, baseline-subtracted flux
	  thisflux = thisflux + centreflux;
	  thisblob->setflux((int)thisflux);
	  thisblob->setsnr((thisflux+centreflux)/(sqrt(nap)*sigma)); // SNR including centre pixel
	  thisblob->settype(2);
	}
      // point-like
      else
	{
	  thisblob->settype(1);
	  deleteblob(thisblob);       // delete point-like sources.. not (mm)
	}
      
      thisblob = nextblob;
    }
  
  thisblob = lastblob;
}

// ---------- Set the gain and other noise statistics parameters ----------

void frameblob::set_gain( double in_gain )
{
  gain = in_gain;
}

void frameblob::set_readout_offset( double in_readout_offset )
{
  readout_offset = in_readout_offset;
}

void frameblob::set_readout_noise( double in_readout_noise )
{
  readout_noise = in_readout_noise;
}

// ---------- Check for multiple sources ----------------------------------

void frameblob::fix_multiple()
{
  int ref_x, ref_y, ref_dist, ref_flux;
  //int maxflux;
  
  bloblist *refblob, *compareblob;
  bloblist *nextcomp;
  
  refblob = compareblob = nextcomp = NULL;
  
  // Traverse blob list
  refblob = firstblob;
  while( refblob != NULL )
    {
      ref_x = (int) refblob->getx();
      ref_y = (int) refblob->gety();
      ref_flux = refblob->getflux();
      
      // Compare each blob to the reference
      compareblob = firstblob;
      while( compareblob != NULL )
	{
	  nextcomp = compareblob->getnextblob();
	  if( compareblob != refblob )
	    {
	      ref_dist = (ref_x-(int)compareblob->getx())*(ref_x-(int)compareblob->getx()) + 
		(ref_y-(int) compareblob->gety())*(ref_y-(int)compareblob->gety());
	      
	      // if this blob is close to the reference and fainter, delete it
	      if( (ref_dist < disttol) && (compareblob->getflux() <= ref_flux) )
		deleteblob(compareblob);
	    }
			compareblob = nextcomp;
	}
      refblob = refblob->getnextblob();
    }
}

double frameblob::computeFocusX()
{
  // Traverse the blob linked list, calculate and return average FWHM of blobs
	// Metric for evaluating the focus - chonko 3/22/05
  
  thisblob = firstblob;
  bloblist *nextblob;
  
   int startx, endx, i, j;
   int center_x, center_y, width_x;
   double centerCounts, background;//, nap;

   double FWHMtotal = 0.;
   double FWHM = 0.;
   int blobCounter = 0;

   	double minValue = 100;	// arbitrary large number
   double maxValue = 0;	

  while( thisblob != NULL )
	{
	  nextblob = thisblob->getnextblob();         // pointer to next blob
	  
	  center_x = (int) thisblob->getx();
	  center_y = (int) thisblob->gety();
	  startx = (int) thisblob->getx() - cenbox/2;  // boundaries
	  endx = startx + cenbox-1;
	  
	  if( startx < 0 ) startx = 0;          // clipping for map boundaries
	  if( endx >= (int) xpix ) endx = xpix-1;
	  
	  width_x = 0.;
	  
	  // Get the center pixel value and background
	  centerCounts = map[center_y*xpix + center_x];
	  background = thisblob->getmean();
	  
	  double threshold = ( centerCounts - background )/2.;
	  
	  int lowerBound = 0; 		
	  int upperBound = 0;
	  bool lowerFound = false;	
	   bool upperFound = false;
	   double pixelValue;
	  double pixelValueOld;
		//  Calculate the FWHM in the x-direction
		for( i=center_x; i>=startx; i-- )
		{
			pixelValue = map[center_y*xpix + i] - background;
			if( !lowerFound && pixelValue < threshold ) {
				lowerFound = true;
				// linear interpolation
				pixelValueOld = map[center_y*xpix + i + 1] - background;
				lowerBound = -(threshold-pixelValue)/(pixelValue-pixelValueOld)+i;
				break;			  
			}
		}
		
		for( j=center_x; j<=endx; j++ )
		  {
		    pixelValue = map[center_y*xpix + j] - background;
			if( !upperFound && pixelValue < threshold ) {
				upperFound = true;
				// linear interpolation
				pixelValueOld = map[center_y*xpix + j - 1] - background;
				upperBound = (threshold-pixelValue)/(pixelValue-pixelValueOld)+j;
				break;			  
			}
		  }
		
		if( !lowerFound ) lowerBound = startx;
		if( !upperFound ) upperBound = endx;
		
		width_x = upperBound - lowerBound;
		
		if( width_x < minValue ) minValue = width_x;
		if( width_x > maxValue ) maxValue = width_x;
		
		// sum the values
		FWHMtotal += width_x;
		
		// go to next blob
		thisblob = nextblob;
		
		//  Increment the number of blobs
		blobCounter++ ;
	}
  
  // average the blob FWHMs and return
  // remove the minimum and maximum values
  if ( blobCounter != 0 ) {
    if ( blobCounter >= 5 ) FWHM = (FWHMtotal-minValue-maxValue)/(blobCounter-2);	  	  
    else FWHM = FWHMtotal/blobCounter;
  }
  
  // if blobCounter = 0 returns 0
  return FWHM;
}

double frameblob::computeFocusY()
{
  // Traverse the blob linked list, calculate and return average FWHM of blobs
  // Metric for evaluating the focus - chonko 3/22/05
  
  thisblob = firstblob;
  bloblist *nextblob;
  
	 int starty, endy, i, j;
	 int center_x, center_y, width_y;
        double centerCounts, background;

	 double FWHMtotal = 0.;
	 double FWHM = 0.;
	int blobCounter = 0;

	double minValue = 100;	// arbitrary large number
	double maxValue = 0;	

	while( thisblob != NULL )
	{
		nextblob = thisblob->getnextblob();         // pointer to next blob
		
		center_x = (int) thisblob->getx();
		center_y = (int) thisblob->gety();
		
		// boundaries
		starty = (int) thisblob->gety() - cenbox/2;
		endy = starty + cenbox-1;
		
		if( starty < 0 ) starty = 0;
		if( endy >= (int) ypix ) endy = ypix-1;
		
		width_y = 0.;
		
		// Get the center pixel value and background
		centerCounts = map[center_y*xpix + center_x];
		background = thisblob->getmean();

		double threshold = ( centerCounts - background )/2.;
		
		 int lowerBound = 0; 		
		int upperBound = 0;
		 bool lowerFound = false;	
		 bool upperFound = false;
	        double pixelValue;
		double pixelValueOld;

		//  Calculate the FWHM in the x-direction
		for( i=center_y; i>=starty; i-- )
		{
			pixelValue = map[i*xpix + center_x] - background;
			if( !lowerFound && pixelValue < threshold ) {
				lowerFound = true;
				// linear interpolation
				pixelValueOld = map[(i+1)*xpix + center_x] - background;
				lowerBound = -(threshold-pixelValue)/(pixelValue-pixelValueOld)+i;
				break;
			}
		}

		for( j=center_y; j<=endy; j++ )
		{
			pixelValue = map[j*xpix + center_x] - background;
			if( !upperFound && pixelValue < threshold ) {
				upperFound = true;
				// linear interpolation
				pixelValueOld = map[(j-1)*xpix + center_x] - background;
				upperBound = (threshold-pixelValue)/(pixelValue-pixelValueOld)+j;
				break;
			}
		}
		
		if( !lowerFound ) lowerBound = starty;
		if( !upperFound ) upperBound = endy;
		
		width_y = upperBound - lowerBound;
		
		if( width_y < minValue ) minValue = width_y;
		if( width_y > maxValue ) maxValue = width_y;
		
		// sum the values
		FWHMtotal += width_y;
		
		// go to next blob
		thisblob = nextblob;
		
		//  Increment the number of blobs
		blobCounter++ ;
	}
	
	// average the blob FWHMs and return
	// remove the minimum and maximum values
	if ( blobCounter != 0 ) {
	  if ( blobCounter >= 5 ) FWHM = (FWHMtotal-minValue-maxValue)/(blobCounter-2);	  	  
	  else FWHM = FWHMtotal/blobCounter;
	}  
	
	// if blobCounter = 0 returns 0
	return FWHM;
}




// --- Set the basic frameblob parameters ---

void frameblob::setXpix(unsigned int xpix_in){
  xpix = xpix_in;
};

void frameblob::setYpix(unsigned int ypix_in){
  ypix = ypix_in;
};

void frameblob::setPlatescale(double platescale_in){
  platescale = platescale_in;
};
