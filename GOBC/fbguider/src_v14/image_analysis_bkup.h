#ifndef IMAGE_ANALYSIS_H_
#define IMAGE_ANALYSIS_H_

#include "parameters.h"
#include <stdint.h>



#define PLATESCALEX 0.823
#define PLATESCALEY 0.823
#define XY_RATIO 1.0
// forward declarations
// NOTE: CNES would prefer that we keep our ROTATOR_OFFSET = 0, they will determine home position
// on their side
#define ROTATOR_OFFSET 0.0

// voltage per arcsecond 11 mV/as 
#define VOLTSARCSEC 0.011

#define ARCSECBINPIX 1.22 
// VOLTSPIX 0.011 is WRONG, but is below for temporary compatibility
// it should be 0.01342. Uncomment to correct - Done
//#define VOLTSBINPIX 0.011 - this is incorrect
# define VOLTSBINPIX 0.01342


typedef struct camera_parameters_t camera_parameters_t;

// possible modes for image analysis -- edit as needed and write the
// necessary logic into the worker function.
// description of the modes below
typedef enum{
  Idle,
  Acquire,
  MultiStarFixed,
  MultiStarFollow
} FBModes;
// Idle: Imaging only. No guiding. Full screen display. 
#define NFBMODES 4

typedef struct attitude_t{ 
  double ra_d, dec_d, pa_d; // Dtu i.e., gondola
  double ra_a, dec_a, pa_a; // Astrometry
  double ra_g, dec_g, pa_g; // uploaded from Ground
  double *ra, *dec, *pa; // pointer to currently used sensor
  char sensor; // 0=gondola, 1=astrometry, 2=uploaded
} attitude_t;

   
// Acquire: Single full device image that locates blobs
// then switch to the N-th blob (as specified), single star pointing. 
// if the N'th blob is not available, it just grabs the brightest. Offset 
// only, no rotation

// MultiStarFixed: Track on multiple stars. Location on device where
// the software searches for the star in subsequent images remains
// fixed. 

// MultiStarFollow: Track on multiple stars. Location on device where
// the software searches for the star in subsequent images is based on
// previous location of star.  




// ATTENTION:
// The following structure is to contain all the data and parameters
// you need to compute offsets, rotations, and the like
typedef struct guide_data_t{
  FBModes fbmode;
  FBModes nextfbmode;
  char changefbmode;
  
  // target coords for the stars, including flux
  float tx[MAXSTARS] , ty[MAXSTARS] , tf[MAXSTARS];
  float tw[MAXSTARS];  
  // which of these are we actually using?
  unsigned char use[MAXSTARS];

  // next set of target coordinates to use
  float nexttx[MAXSTARS], nextty[MAXSTARS], nexttf[MAXSTARS];
  float nexttw[MAXSTARS];
  unsigned char nextuse[MAXSTARS];
  char nexttargets, updatetargets;

  // currently measured star positions
  float x[MAXSTARS], y[MAXSTARS], f[MAXSTARS];
  // which of these have computed as 'valid'
  unsigned char valid[MAXSTARS];

  // last measured positions
  float lastx[MAXSTARS], lasty[MAXSTARS], lastf[MAXSTARS];
  unsigned char lastvalid[MAXSTARS];
  // rotation center coordinates
  float rx, ry;
  
  // next positions of the stars to consider:
  float nextx[MAXSTARS], nexty[MAXSTARS], nextf[MAXSTARS];
  unsigned char nextvalid[MAXSTARS];
  char nextstars, updatestars;


  // information on the field
  double field_ra, field_dec, field_pa;
  double field_el, field_az, field_rot;
  double next_field_ra, next_field_dec, next_field_pa;
  // field center coordinates in detector coordinates
  float field_cx, field_cy;
  
    

  float snrcut;
  uint16_t width, height;
  // frameblob parameters
  unsigned int fb_bits;
  uint16_t fb_cam_sat;
  float fb_plate_scale;
  unsigned int fb_coarse_grid;
  unsigned int fb_cen_box;
  unsigned int fb_ap_box;
  float fb_sigma_thresh;
  int fb_dist_tol;
  
  // don't touch
  int bufidx;
  unsigned long int framenumber;
  uint16_t *buf;
  uint16_t *boxbuf;
  char initialized;
} guide_data_t;



// box structure
typedef struct boxes_t{
  char initialized;
  uint16_t *boxes[MAXSTARS];
  int halfwidth, halfheight;
  int size;
  int pitch; 
  uint16_t star_cut;
  int *xmult, *ymult, *type, *pixoff, star_cut_count;
  int xsqsum, ysqsum;
  int star_cut_star_count, star_cut_bg_count;
  uint16_t cx[MAXSTARS], cy[MAXSTARS];
} boxes_t;

namespace img{
  
  // worker thread. 
  void *imageworker(void *data);
  

  // below are general utility functions
  // initialize and close the image analysis stuff
  void imginit(int w, int h, int boxsize);
  void imgclose();
  // this spins off the thread
  void process_frame(camera_parameters_t *campar, int bufidx);
  // image centroiding functions
  void acq_func(uint16_t *buf, float *x, float *y, float *flux, 
		unsigned char *valid, unsigned char *nstars, int index);
  // allocate and deallocate memory for boxes
  void init_boxes(int halfsize);
  void free_boxes();
  // copy boxes from buf to the boxes structure.
  void extract_boxes(uint16_t *buf, float *cx, float *cy, 
		     unsigned char *success, unsigned char *use);
  // find star centroids within boxes (incl. helper functions)
  void boxes_compute_star_cut(int scut);
  void boxes_destroy_star_cut();
  void boxes_redo_star_cut(int starcut);
  void centroid_boxes(float *cx, float *cy, float *cf,
		      float *sigx, float *sigy, 
		      unsigned char *valid, unsigned char *use);
  
  // data access functions
  FBModes getFBMode();
  void setFBMode(FBModes fbm);
  
  long getFramenumber();

  int compute_mapping(float *x0, float *y0,
                      float *x1, float *y1,
                      float *weight, unsigned char *valid,
		      unsigned char *use, 
		      char points,
		      float *dx, float *dy, float *drot);
  

  void remove_distortion(float *x, float *y,
                         float *v, float *u,
                         int count);

  void add_distortion(float *u, float *v,
                      float *x, float *y,
                      int count);
  
  // load the next target file
  void load_next_target(unsigned short value);

  // covert the next target values to the current star positions (i.e., 
  // tell the computer where to find the stars that are supposed to be on target)
  void convert_next_target_to_next_star(float dx, float dy, float drot);
  void convert_next_target_to_next_star_sky(double dra, double ddec, float drot);
  
  void set_next_targets(float *sx, float *sy, unsigned char *suse, double ra, double dec, double rot);
  void set_next_single(float sx, float sy, unsigned char suse);
  void set_next_stars(float *sx, float *sy, unsigned char *svalid);

  // switch next targets and spots to current and continue guiding... hopefully. 
  void switch_next_to_current();
  
  // change flags
  void set_change_flag(char nxtargs, char nxstars, char updtargs, char updstars);

  void adjust_targets(float dx, float dy, float drot);
  void set_target_points(short *tx,short *ty);


  void update_targets();
  void update_stars();

  // given the orientation of the spectrograph, 
  void compute_elce(double specrot_radians, 
		     float specrot_radians_offset,
		     float dx,
		     float dy,
		     float *el,
		     float *ce);

  void set_attitude_sensor(char sensor);
  void upload_attitude(double ra_, double dec_, double pa_);
  void gondola_attitude(double ra_, double dec_, double pa_);
  void astrometry_attitude(double ra_, double dec_, double pa_);
  


}; // namespace img




#endif // IMAGE_ANALYSIS_H_
