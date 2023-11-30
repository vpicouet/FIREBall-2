#ifndef FBCAMERA_H_
#define FBCAMERA_H_

// standard integer types uint16_t, etc. 
#include <stdint.h>

// simulate mode? TRUE/FALSE definitions
// other general definitions
#include "parameters.h"
#include "fblog.h"
#include "stacking.h"

// camera types 
#include "../lib/pco_include/VersionNo.h"
#include "../lib/pco_classes/pco_cl_include.h"
#include "../lib/pco_classes/Cpco_cl_com.h" 
#include "../lib/pco_classes/Cpco_me4.h"
//#include "../siso_include/clser.h"
//nicole edit 6/15
#include "../lib/siso_include/clser.h"

// various defaults. you are responsible for making sure that they all
// play nice together. While the defaults cannot be modified, commands
// can be sent to change the values

// camera pixel rate
//#define CAMERA_PIXELRATE 286000000
#define CAMERA_PIXELRATE 95300000

// camera binning
#define CAMERA_BINNING 2
// camera x size
#define CAMERA_XSIZE (2560/CAMERA_BINNING)
// camera y size
#define CAMERA_YSIZE (2160/CAMERA_BINNING)

// minimum and maximum exposure time 
#define EXP_MIN 1
#define EXP_MAX 2000
#define EXP_CUTOFF 33

// defines for the PCO EDGE system
#define FATAL 0xF0000000
#define MAX_RETRY 10
#define WARNING 0x20000000
#define OK 0x00000000

// How many memory buffers to sycle through?
#define IMAGE_BUFFERS 3

// filename for framenumber and how often to save
#define FRAMENUMBER_FILENAME DEFAULT_LOGDIR"framenumber.dat"
#define FRAMENUMBER_SAVE_FREQUENCY 10000


// forward declarations


// definition of camera parameters. 
typedef struct camera_parameters_t{
  // chip width, and height
  uint16_t width, height;
  // timing things
  uint32_t exptime;
  int trigtime;
  // image buffers
  uint16_t *picbuf[IMAGE_BUFFERS];
  float *stackbuf;
  // camera, ccd, and power supply temperatures
  //  short Tccd, Tcam, Tps;
  // stacking?
  char stacking;
  char solvestack;
  unsigned short stack_size, stack_count;
  // framenumber
  unsigned long int framenumber;
  // temperatures
  short ccdtemp, camtemp, pstemp;
} camera_parameters_t;

typedef struct new_exptime_t{
  char flag;
  uint32_t exptime;
} new_exptime_t;

typedef struct new_stack_t{
  char flag;
  unsigned char stack;
  unsigned short stacksize;
  unsigned char solve;
} new_stack_t;


// camera class
class fbcamera{
 private:
  CPco_cl_com camera;
  CPco_me4_edge grabber;
  PCO_SC2_CL_TRANSFER_PARAM clpar;
  // status bits 
  char camera_on;
  char camera_open;  // is the camera open? 
  char camera_acquiring; // is the camera acquiring?
  
  // internal buffer address
  uint16_t *adr;
  // timeout for acquisition variable
  int timeout;
  // how many pictures to grab at once
  int picnum;
  // pixel rate
  uint32_t pixelrate;
  //binning
  uint32_t binx,biny;
  //delay
  uint32_t delay;
 public:
  // error value
  uint32_t err;

  // various camera parameter values
  camera_parameters_t camera_parameters;

  // structure for passing a new exposure time
  new_exptime_t new_exptime;

  // structure for stacking
  stacking_structure_t stack_structure;

  // constructor
  fbcamera(char camon);
  
  // destructor
  ~fbcamera();

  // all parameters are set in the constructor, only exposure time
  // can be modified
  int set_exposure(uint32_t exposure);

  // start and stop acquisition
  int start();
  int stop();

  // take an image and increment the counter.
  // returns the index of the buffer written to
  int grab_single();

  // get the temperatures from the camera and fill out appropriate parameters
  int get_temperature();

  // functions not included in the PCO classes, but needed and with
  // serial access available at a lower level
  uint32_t GetBinning(uint16_t *binningx, uint16_t *binningy);
  uint32_t SetBinning(uint16_t binningx, uint16_t binningy);
  uint32_t SetROI(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

  // load and/or save the counter
  // the frame number will be saved every 1000 frames and on program
  // exit. It will be loaded whenever the program starts. 
  int load_framenumber();
  int save_framenumber();

  // we may want to stack some images; this function adds current
  // image to an existing stack, or starts a new stack, or writes the
  // stack to disk and executes an astrometry 
  int stack_and_solve_image(uint16_t *data);

  // do we stack? if so, how many images (1-100). Do we solve, if so, how?
  // 0 = no solve
  // 1 = use ra/dec proximity search
  // 2 = lost in space
  int config_stack_and_solve(unsigned char stack, unsigned short stacksize, unsigned char solve);

  // functions to request/update exposure time
  friend int update_exptime(fbcamera *fbcam);
  
  // functions to request/update stack setting
  friend int update_stack(fbcamera *fbcam);

  // functions to define whether the camera is being used or not
  void set_camera_on(char camon);
  char get_camera_on();
  
}; // class fbcam


// functions to request/update exposure time
int request_new_exptime(uint32_t exptime);

// functions to request/update stack
int request_new_stack(unsigned char stack,
		      unsigned short stacksize,
		      unsigned char solve);

// function to access temperatures
void retrieve_temperatures(short *ccdtemp,
			   short *camtemp,
			   short *pstemp);

// function to access exposure time
void retrieve_exptime(int *exptime);




#endif // FBCAMERA_H_
