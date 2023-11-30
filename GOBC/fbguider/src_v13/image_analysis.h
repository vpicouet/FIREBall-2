#ifndef IMAGE_ANALYSIS_H_
#define IMAGE_ANALYSIS_H_

#include "parameters.h"
#include <stdint.h>



#define PLATESCALEX 0.823
#define PLATESCALEY 0.823
#define XY_RATIO 1.0
// forward declarations
#define ROTATOR_OFFSET 0.0

// voltage per arcsecond 11 mV/as 
#define VOLTSARCSEC 0.011

#define ARCSECBINPIX 1.22 
// VOLTSPIX 0.011 is WRONG, but is below for temporary compatibility
// it should be 0.01342. Uncomment to correct - Done
//#define VOLTSBINPIX 0.011 - this is incorrect
# define VOLTSBINPIX 0.01342

// new encoding algorithm
#define GE_THETALIM 20.0
#define GE_LSB1 0.020
#define GE_LSB2 3.416
#define GE_NLIM GE_THETALIM/GE_LSB1
#define VOLTCONV  5.0/2048.0   // volts per digital pixel

#define TAGNUMBER_FILENAME DEFAULT_LOGDIR"imgtag.dat"

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


// The structure below contains a subset of the guide_data_t type
// members that we want to save in the binary file. 
typedef struct guide_log_data_t{ 
  FBModes fbmode;
  float tx[MAXSTARS], ty[MAXSTARS], tf[MAXSTARS];
  float x[MAXSTARS], y[MAXSTARS], f[MAXSTARS];
  unsigned char use[MAXSTARS], valid[MAXSTARS];
  float dx,dy,drot;
  unsigned long int framenumber;
} guide_log_data_t;

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
  float sigx[MAXSTARS], sigy[MAXSTARS];
  // error value calculations -- nicole 
  float dx, dy, drot; 
  
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
  float star_sig_min, star_sig_max;
  uint16_t width, height;
  // frameblob parameters
  unsigned int fb_bits;
  uint16_t fb_cam_sat;
  float fb_plate_scale;
  unsigned int fb_coarse_grid;
  unsigned int fb_cen_box;
  unsigned int fb_ap_box;
  float fb_sigma_thresh;
  double fb_gain;
  double fb_offset;
  double fb_rn;
  int fb_dist_tol;

  unsigned char maxstars;
  
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
		unsigned char *valid, unsigned char *nstars, int index, double *xfwhm, double *yfwhm);
  
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

  void centroid_boxes_1(float *cx, float *cy, float *cf,
		      float *sigx, float *sigy, 
		      unsigned char *valid, unsigned char *use);

  void centroid_boxes_2(float *cx, float *cy, float *cf,
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
  
  // error signal -- nicole 
  double ge_ang2volts(double dan); 

  // load the next target file
  void load_next_target(unsigned short value);

  // magnitude from blob flux 
  void mag_func(float *vmag, float *flux, int& my_exposure); 

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
  void adjust_single_star(unsigned char starid, float newx, float newy);
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
  
  // 170727 -- matmat
  void change_used_stars(unsigned char usemask);
  void change_guide_star_sigma(unsigned char thresh, unsigned short sigmin, unsigned short sigmax);

  void get_thrsig(unsigned char *thresh, unsigned short *smin, unsigned short *smax);
  void get_target_pointers(float *tx, float *ty);

  void get_star_pointers(float *cx, float *cy, float *cf, float *sx, float *sy);
  void get_star_usevalid(unsigned char *use, unsigned char *valid);


  void set_fb_gain(float gain);
  void set_fb_satval(uint16_t sarval);
  void set_fb_thresh(float thresh);
  void set_fb_distol(int distol);
  void set_fb_gridsize(unsigned int gridsize);
  void set_fb_cenbox(unsigned int cenbox);
  void set_fb_apbox(unsigned int apbox);
  void set_fb_offset(float offset);
  void set_fb_rn(float rn);
  void get_fb_vals(float *gain, uint16_t *satval,
		   float *thresh, int *distol,
		   unsigned int *gridsize, unsigned int *cenbox,
		   unsigned int *apbox, float *offset,
		   float *rn);
  void get_guide_stats(float *dx, float *dy, float *dr, float *rx, float *ry, float *rr);
  void compile_guide_stats(float dx, float dy, float dr);
  void reset_fb();
  void setMaxStars(unsigned char mx);
  // write the guide data to disk as binary.
  void write_guide_data(guide_data_t *gdt);
  void write_guide_text_data(guide_data_t *gdt);

  void subtract_rownoise(uint16_t *buf);

  void load_median_image();
  void subtract_median(uint16_t *buf);

  void set_subtract(unsigned char s);
  unsigned char get_subtract();

  void set_centroiding_algorithm(unsigned char s);
  unsigned char get_centroiding_algorithm();

  void set_image_tag(unsigned short s);
  unsigned short get_image_tag();

  void set_image_save_period(unsigned short sp);
  unsigned short get_image_save_period();

  void get_profiles(unsigned char starid, void *spt);
  void set_profiles(unsigned char starid, unsigned char val, unsigned char used, float *xprofile, float *yprofile, float *xfit, float *yfit,long frameno);

  void find_minmax(float *xprof, float *yprof, int size, int smooth, float *xmin, int *xminidx, float *xmax, int *xmaxidx, float *ymin, int *yminidx, float *ymax, int *ymaxidx);


  void refine_targets();
  void start_refine_targets(unsigned char count);
  
  int load_tagnumber();
  int save_tagnumber();
  void start_construct_min_image(unsigned char count);
  void construct_min_image(uint16_t *ptr);

  void switch_to_next_targets();


  int imgboxsize();
  int get_env_boxsize();
}; // namespace img




#endif // IMAGE_ANALYSIS_H_
