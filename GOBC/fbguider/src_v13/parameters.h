
#ifndef PARAMETERS_H_
#define PARAMETERS_H_


// general defines
#define TRUE 1
#define FALSE 0 

// system configuration
// are we making use of the text or binary logs? 
#define TXT_LOG_ON TRUE
#define BIN_LOG_ON TRUE
#define IMG_SAVE_ON TRUE
// we save every what image?  At typical rate, this is once every 15 seconds
// Uncompressed this is 20 MB/Min or 1.2 GB/HR  --- was 450
#define IMG_SAVE_PERIOD 150

// is the camera being used? 
#define CAMERA_ON TRUE
//#define CAMERA_ON FALSE

// are we expecting messages from or sending messages to the 
// french computer? 
#define MPF_ON TRUE
//#define MPF_ON FALSE

// are we using the DIO card at all?
#define CARD_ON TRUE
//#define CARD_ON FALSE
// are we sending analog messages to the gondola? 
#define ANALOG_SIGNALS_ON TRUE
// #define ANALOG_SIGNALS_ON FALSE
// are we controlling shutters and lamps?
#define DIGITAL_OUT_ON TRUE
// #define DIGITAL_OUT_ON FALSE
// are the analog input signals on?
#define ANALOG_IN_ON TRUE
// #define ANALOG_IN_ON FALSE
// are the digital input signals on?
#define DIGITAL_IN_ON TRUE
// #define DIGITAL_IN_ON FALSE

// are we expecting messages from or sending messages to the 
// detector computer? 
//#define DET_ON FALSE
#define DET_ON TRUE

// are we expecting messages from or sending messages to the
// ground computer? 
#define GND_ON TRUE

// are we using the video? 
//#define VIDEO_ON TRUE
#define VIDEO_ON TRUE
// what video mode? (full screen or not?)
//#define FB_WINDOW_MODE SDL_WINDOW_FULLSCREEN
#define FB_WINDOW_MODE SDL_WINDOW_SHOWN
// How often do we want video update, define video update period in frames (min=1)
#define VIDEO_UPDATE_PERIOD 450
#define USEFONTS
#undef USEFONTS

// EL - CE Rose direction, position, and size
#define DIRECTION_EL 1
#define DIRECTION_CE 1
#define ROSE_X 900
#define ROSE_Y 800

// focus stage control on? 
#define FOCUS_STAGE_ON TRUE
//#define FOCUS_STAGE_ON FALSE

// mask stage control on?
#define MASK_STAGE_ON TRUE
//#define MASK_STAGE_ON FALSE

// nominal linear stage positions 
#define AXISANOMINAL 12.50
#define AXISBNOMINAL 13.75
#define AXISCNOMINAL 12.65

// nominal carousel positions
#define CARPOS0 -1
#define CARPOS1 39.0
#define CARPOS2 79.0
#define CARPOS3 119.0
#define CARPOS4 159.0
#define CARPOS5 -161.0
#define CARPOS6 -121.0
#define CARPOS7 -81.0
#define CARPOS8 -41.0


// CAMERA INITIALIZATION PARAMETERS
// for binning and pixelrate, check fbcamera.h
#define CAMINIT_STACKING_SIZE 10

#define CAMINIT_TIMEOUT 10
#define CAMINIT_PICNUM 1

// Was 100, changed both to 33 for 30 Hz
#define CAMINIT_EXPTIME 37
#define CAMINIT_TRIGTIME 37 

//#define CAMINIT_MIN_PERIOD 30

// Some generally useful parameters

// what is the max number of stars we can track on? 
#define MAXSTARS 8
#define STARS 8
//#define MAXSTARS 2
//#define STARS 2

// chip rotation center
//#define ROTATION_CENTER_X 640.0
#define ROTATION_CENTER_X 1295.0
//#define ROTATION_CENTER_X 1040.0
#define ROTATION_CENTER_Y 557.5


// what is the initial signal-to-noise ratio we will work with 
#define IMG_SNR 3.0
// box size can only be changed at compile time
// #define IMG_BOXSIZE 35
#define IMG_BOXSIZE 35
// #define IMG_BOXSIZE img::imgboxsize()
#define IMG_BOXSIZE_MIN 10
#define IMG_BOXSIZE_MAX 50
#define IMG_BOXSIZE_DEFAULT 35
#define BAD_COUNTER 5
// size of cut-out for a star in a box (5 is initial guess)
// what is the max? 
#define BOX_STAR_CUT 3
#define MAX_STAR_CUT 15

// star sigmas min,max
#define STAR_SIG_MIN 1.0
#define STAR_SIG_MAX 25.0

// FRAMEBLOB PARAMETERS
#define FB_BITS        16
#define FB_CAM_SAT     65535
#define FB_PLATE_SCALE 1.0
// below was 50 on 170830
#define FB_COARSE_GRID 50
// FB_CEN_BOX was 50 on 170801
#define FB_CEN_BOX     13
#define FB_AP_BOX      4
#define FB_SIGMA_THRESH 1.4
// below was 30 on 170830
#define FB_DIST_TOL    35
#define FB_GAIN 1
// below was 10 on 170830
#define FB_RN 15
//below was 100 on 170830
#define FB_OFFSET 360

#define BADCOUNTER 5

// general debug line
#define DEBUG 
#undef DEBUG

#ifdef DEBUG 
#define DEBUGLINE fprintf(stdout,"#%s; %s; %d\n",__FILE__,__FUNCTION__,__LINE__)

#define DEBUGPRINT(...) fprintf(stdout,"In %s; %s; %d: ",__FILE__,__FUNCTION__,__LINE__); \
  fprintf(stdout,__VA_ARGS__)
#endif // debug

#ifndef DEBUG
#define DEBUGLINE  
#define DEBUGPRINT(...) 
#endif // debug




#endif // PARAMETERS_H_
