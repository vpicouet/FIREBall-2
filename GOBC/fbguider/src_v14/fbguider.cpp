#include <cstdio>
#include <cstdlib>
#include <sys/unistd.h>

#include <signal.h>

#include "fblog.h"
#include "fbcamera.h"
#include "fbvideo.h"
#include "image_analysis.h"
#include "timing.h"
#include "communication.h"
#include "adio.h"
#include "ground_communication.h"

// set this negative for an indefinite loo
#define REPEAT 5000000
//#define REPEAT 5000

// am i observing from 3000 miles?
#define REMOTE 1
#undef REMOTE
// REMOTESLEEP in microseconds
#define REMOTESLEEP 5000000

#define TESTING 1
#undef TESTING




static volatile int keep_running=1;

void intHandler(int dummy){
  fblog::logmsg("CTRL-C caught");
  fprintf(stderr,"CTROL-C caught");
  keep_running = 0;
};

int main(int argc, char *argv[]){
  int idx,i;
  char camon;
  int local;
  const char* version_string = VERSION_STRING;

  // getopt parameters
  //  extern char *optarg;
  //  extern int optind;
  //  int cam_flag, port_flag;
    int c; 
  
  // open the logs
  start_time();
  fblog::logopen();

  camon=1;
  local=0;
  // process the arguments 
  // this is *very* strict...
  //  if ( argc == 2 ){
  //    if (strcmp(argv[1],"-nocam")==0) camon=0;
  // };

  /*** 
       Adding options here. 
   ***/
  while ((c = getopt(argc, argv, "nl")) != -1)
    switch (c) {
    case 'n':
      camon = 0;
      fblog::logmsg("Camera not requested (-n)");
      break;
    case 'l':
      fblog::logmsg("Local port operation. Makeserial must be running.");
      local = 1;
      break;
    case '?':
      fblog::logmsg("Invalid option (-%c)! ",c);
      break;
    default:
      fblog::logmsg("Invalid option (-%c)",c);
      break;
    }

  fblog::logmsg("Code version: %s",version_string);
  

  camon = CAMERA_ON & camon;
  printf("Camera on: %d\n",camon);
  fblog::logmsg("Camera on flag: %d",camon);

  
  // initialize communications
  //  fbcomm::commopen();
  communications_start(local);

  // open the ports
  adio::open_ports();

  // initialize the camera 
  fbcamera fbcam(CAMERA_ON & camon);

  //  fbcam.set_camera_on(CAMERA_ON & camon);
  //  DEBUGPRINT("this is a debug statement!\n");

  // initialize image processing
  img::imginit(fbcam.camera_parameters.width, 
	       fbcam.camera_parameters.height,
	       IMG_BOXSIZE);


  // initialize the video
  fbvideo::video_start(fbcam.camera_parameters.width, 
		       fbcam.camera_parameters.height, 
		       IMG_BOXSIZE*2+1);

   char txt[255];
  sprintf(txt,"Hello Earthling!");
  ground::send_text(txt);
  sprintf(txt,"Code version: %s. camon=%d. local=%d",version_string,camon,local);
  ground::send_text(txt);
  // start counter
  i = 0;
      DEBUGLINE;
  // set up signal handling
  signal(SIGINT, intHandler);

  // debug stuff
  // debug stuff
  //  fbcam.camera_parameters.stacking = 1;
  //  fbcam.camera_parameters.stack_size = 20;
  //  fbcam.camera_parameters.stack_count = 0;

  //  adio::digital_on((digital_channel_t) chan);
  adio::analog_out_both(0, 0);
  DEBUGLINE;
  while(keep_running){
#ifndef TESTING
    idx=fbcam.grab_single();
#endif
    //    fbcam.camera_parameters.picbuf[idx] = (uint16_t *) img1;

#ifdef REMOTE
    DEBUGPRINT("i=%d et=%d\n",i,fbcam.camera_parameters.exptime);
#endif 
#ifndef TESTING
    img::process_frame(&fbcam.camera_parameters, idx);
    if (fbcam.get_camera_on()){
      fbcam.stack_and_solve_image(fbcam.camera_parameters.picbuf[idx]);
    };
#endif
#ifdef REMOTE
    usleep(REMOTESLEEP);
#endif
#ifndef TESTING
    if ((i % 100) == 2){
      fbcam.get_temperature();
    }
#if CAMERA_ON == TRUE
#ifdef IMG_SAVE_ON
    //    if ((i % IMG_SAVE_PERIOD) == 5){
    if ( ( img::get_image_save_period() > 0 ) && ((i % img::get_image_save_period()) == 5) ){
      request_new_stack(1,1,10);
    }
#endif
#endif 
#endif 
    if( (i % 30) == 2 ){
      DEBUGPRINT("Temps: CCD=%d, CAM=%d, PS=%d\n",
		 fbcam.camera_parameters.ccdtemp, 
		 fbcam.camera_parameters.camtemp, 
		 fbcam.camera_parameters.pstemp);
      fblog::logmsg("CAMT %d, %d, %d",
		    fbcam.camera_parameters.ccdtemp, 
		    fbcam.camera_parameters.camtemp, 
		    fbcam.camera_parameters.pstemp);
    };
    i++;

    // update a few things that may have come in over the serial link
    
#ifndef TESTING
    // has an exposure time been requested?
    update_exptime(&fbcam);
    // has a new stack been requested? 
    update_stack(&fbcam);
#endif 

    if (i > REPEAT && REPEAT > 0) keep_running = 0;   
  };

  adio::close_ports();

  //  for(int jdx=0;jdx<3;jdx++)
  //    fbcam.camera_parameters.picbuf[jdx] = pimg[jdx];

  usleep(5000000);

  // clean up the image structures
  img::imgclose();
  // close the video
  fbvideo::video_stop();
  // close communications
  communications_stop();
  // close the logs
  usleep(2000000);
  fblog::logclose();
  //  usleep(1000000);
  return 0;
};
