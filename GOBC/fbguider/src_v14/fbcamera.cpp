#include <cstdlib>
#include <cstdio>
#include <stdint.h>

#include "../lib/pco_include/VersionNo.h"
#include "../lib/pco_classes/pco_cl_include.h"
#include "../lib/pco_classes/Cpco_cl_com.h"
#include "../lib/pco_classes/Cpco_me4.h"
#include "../siso_include/clser.h"

#include "fbcamera.h"
#include "fblog.h"

#include "nocamera.h"

// structure for passing a new exposure time between modules/threads
new_exptime_t new_exptime;

// thread for stacking, saving, and solving.
new_stack_t new_stack;

// structure for accessing camera parameters from other places
camera_parameters_t *fbcamera_parameters;

pthread_t stack_and_save_thread;


//constructor. Opens camera and grabber and gets them ready. 
fbcamera::fbcamera(char camon){
  err = 0;   // reset the error variable
  int att=1; // attempts
  uint32_t width,height; // width and height


  // exptime updating stuff
  new_exptime.flag = FALSE;
  new_exptime.exptime = CAMINIT_EXPTIME;

  fbcamera_parameters = &camera_parameters;

  // populate the camera parameters
  picnum = CAMINIT_PICNUM;
  timeout = CAMINIT_TIMEOUT;
  camera_parameters.exptime = CAMINIT_EXPTIME;
  camera_parameters.trigtime = CAMINIT_TRIGTIME;
  
  if(CAMINIT_EXPTIME < 33) { 
    delay = 33-CAMINIT_EXPTIME;
  } else { 
    delay = 0;
  };

  camera_parameters.width = CAMERA_XSIZE;
  camera_parameters.height = CAMERA_YSIZE;
  width = camera_parameters.width;
  height = camera_parameters.height;
  camera_parameters.stacking = FALSE;
  camera_parameters.stack_size = CAMINIT_STACKING_SIZE;
  camera_parameters.stack_count = 0;

  binx = CAMERA_BINNING;
  biny = CAMERA_BINNING;
  pixelrate = CAMERA_PIXELRATE;

  load_framenumber();

  camera_on = camon;
  
  printf("camera on = %d\n",camera_on);
  
  // what do we do if camera is not defined? 
  if(camera_on) { 
    camera_open = 0;
    camera_acquiring = 0;
    // camera is present
    // try to open camera
    for (att=1; att <= MAX_RETRY && err != FATAL; att++){
      fblog::logmsg("Attempt #%3d of opening camera.",att);

      // try to open camera
      err = camera.Open_Cam(0);
      if (err != PCO_NOERROR){
	fblog::logerr("Failed to open camera [0x%x]",err);
	continue;
      }; // !PCO_NOERROR
      fblog::logmsg("Camera opened.");


      // make sure camera recording state is OFF
      err = camera.PCO_SetRecordingState(0);
      if (err != PCO_NOERROR){
	fblog::logerr("Recording state OFF failed [0x%x]",err);
      } else {
	fblog::logmsg("Recording state OFF");
      }; // PCO_NOERROR

      // turn off timestamp mode
      err = camera.PCO_SetTimestampMode(0);
      if (err != PCO_NOERROR){
	fblog::logerr("Timestamp Mode OFF failed [0x%x]",err);
      } else {
	fblog::logmsg("Timestamp Mode OFF");
      }; // PCO_NOERROR

      // set the timebase to milliseconds
      err = camera.PCO_SetTimebase(2,2);
      if (err != PCO_NOERROR){
	fblog::logerr("Setting timebase failed [0x%x]",err);
      } else {
	fblog::logmsg("Set timebase");
      }; // PCO_NOERROR


      // arm the camera
      err = camera.PCO_ArmCamera();
      if (err != PCO_NOERROR){
	fblog::logerr("Arming camera failed [0x%x]",err);
      } else {
	fblog::logmsg("Armed camera");
      }; // PCO_NOERROR
     

      // opening frame grabber
      err = grabber.Open_Grabber(0);
      if (err != PCO_NOERROR){
	fblog::logerr("Failed to open grabber [0x%x]",err);
	fblog::logmsg("Closing camera.");
	// since there is an error, we need to close camera before we try to reopen things
	camera.Close_Cam();
	continue;
      }; // !PCO_NOERROR 
      fblog::logmsg("Grabber opened.");

      // OK. Camera and grabber are open. Configure the camera. 



      // arm the camera
      err = camera.PCO_ArmCamera();
      if (err != PCO_NOERROR){
	fblog::logerr("Arming camera failed [0x%x]",err);
      } else {
	fblog::logmsg("Armed camera");
      }; // PCO_NOERROR

      
      camera.PCO_SetRecordingState(0);

      // set the delay and exposure time 
      err = camera.PCO_SetDelayExposure(delay, 
					camera_parameters.exptime);
      if (err != PCO_NOERROR){
	fblog::logerr("Set delay+exposure [%d, %d] failed [0x%x]",delay, camera_parameters.exptime,err);
      } else {
	fblog::logmsg("Set delay+exposure");
      }; // PCO_NOERROR


      // set binning to 2x2
      err = SetBinning(binx,
		       biny);
      if (err != PCO_NOERROR){
	fblog::logerr("Set binning failed [0x%x]",err);
      } else {
	fblog::logmsg("Set binning");
      }; // PCO_NOERROR

      // set dimensions correctly, otherwise camera will complain
      err = SetROI(1, 1, camera_parameters.width, camera_parameters.height);
      if (err != PCO_NOERROR){
	fblog::logerr("Set ROI failed [0x%x]",err);
      } else {
	fblog::logmsg("Set ROI");
      }; // PCO_NOERROR


      err = camera.PCO_ArmCamera();
      camera.PCO_SetRecordingState(0);
      // set pixel rate
      err = camera.PCO_SetPixelRate(pixelrate);
      if (err != PCO_NOERROR){
	fblog::logerr("Set pixelrate failed [0x%x]",err);
      } else {
	fblog::logmsg("Set pixelrate [%d]",pixelrate);
      }; // PCO_NOERROR

      // arm the camera
      err = camera.PCO_ArmCamera();
      if (err != PCO_NOERROR){
	fblog::logerr("Arming camera failed [0x%x]",err);
      } else {
	fblog::logmsg("Armed camera");
      }; // PCO_NOERROR

      // get the image size 
      err = camera.PCO_GetActualSize(&width, &height);
      if (err != PCO_NOERROR){
	fblog::logerr("GetActualSize failed [0x%x]",err);
      } else {
	fblog::logmsg("Got Actual Size [%d x %d]",width,height);
	camera_parameters.width = width;
	camera_parameters.height = height;
      }; // PCO_NOERROR

      // get pixel rate
      err = camera.PCO_GetPixelRate(&pixelrate);
      if (err != PCO_NOERROR){
	fblog::logerr("GetPixelRate failed [0x%x]",err);
      } else {
	fblog::logmsg("Got PixelRate [%d]",pixelrate); 
      }; // PCO_NOERROR

      // compute data format 
      if((width>1920)&&(pixelrate>=286000000))        {
            clpar.DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x12;
      } else { 
            clpar.DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x16;
      }; //computing data format

      // transfer setting parameters
      err = camera.PCO_SetTransferParameter(&clpar, sizeof(clpar));
      if (err != PCO_NOERROR){
	fblog::logerr("SetTransferParameter failed [0x%x]",err);
      } else {
	fblog::logmsg("Set Transfer Parameter");
      }; // PCO_NOERROR


      // re-arm the camera
      err = camera.PCO_ArmCamera();
      if (err != PCO_NOERROR){
	fblog::logerr("Arming camera failed [0x%x]",err);
      } else {
	fblog::logmsg("Armed camera");
      }; // PCO_NOERROR
      
      // set data format 
      err = grabber.Set_DataFormat(clpar.DataFormat);
      if (err != PCO_NOERROR){
	fblog::logerr("Set_DataFormat failed [0x%x]",err);
      } else {
	fblog::logmsg("Set Data Format");
      }; // PCO_NOERROR
      
      // set grabber image size
      err = grabber.Set_Grabber_Size(width,height);
      printf("%d %d\n",width,height);
      if (err != PCO_NOERROR){
	fblog::logerr("Set_Grabber_Size failed [0x%x]",err);
      } else {
	fblog::logmsg("Set Grabber Size");
      }; // PCO_NOERROR

      // allocate buffers
      err = WARNING;
      for(int i=20; i>0 && err != OK; i--){
	err = grabber.Allocate_Framebuffer(i);
	if (err != PCO_NOERROR){
	  // if there was an error
	  if (i > 1){
	    fblog::logerr("Could not allocate buffer for frame %3d [0x%x]",i,err);
	  } else {
	    fblog::logerr("Could not allocate any buffer. [0x%x]",err);
	    fblog::logmsg("Turning off recording state");
	    camera.PCO_SetRecordingState(0);
	    fblog::logmsg("Closing grabber.");
	    grabber.Close_Grabber();
	    fblog::logmsg("Close camera.");
	    camera.Close_Cam();
	    // another failed attempt
	    continue;
	  };
	} else {
	  // if there was no error
	  fblog::logmsg("Buffer %3d allocated.",i);
	}; //PCO_NOERROR

      }; // framebuffer allocation

      // set recording state to ON
      err = camera.PCO_ArmCamera();
      err = camera.PCO_SetRecordingState(0);

      // exit the loop if everything is OK.
      if (err == OK) break;

    }; // for (att=1 ...)
    
    // nope. doesn't work.
    
    if (att == MAX_RETRY && err != OK){
      fblog::logerr("Reached maximum number of attempts to initialzie camera");
      camera_on = FALSE;
    } else { 
      // allocate our own circular buffer
      for (int idx=0;idx<IMAGE_BUFFERS;idx++){
	camera_parameters.picbuf[idx] = 
	  (uint16_t *)malloc(camera_parameters.width*camera_parameters.height*sizeof(uint16_t));
      };
      camera_parameters.stackbuf = 
	(float*)malloc(camera_parameters.width*camera_parameters.height*sizeof(float));

      camera_open = TRUE;
    }; // good, initialization worked. 

  }; // CAMERA_ON

  // what do we do if the camera is _not_ on?
  if (!camera_on){
    // allocate our own circular buffer
    for (int idx=0;idx<IMAGE_BUFFERS;idx++){
      camera_parameters.picbuf[idx] = 
	(uint16_t *)malloc(camera_parameters.width*camera_parameters.height*sizeof(uint16_t));
    };
    nocamera::init(camera_parameters.picbuf, camera_parameters.width, camera_parameters.height);
  }; // CAMERA_OFF

}; //fbcamera::fbcamera();


//destructor
// add error checking? 
fbcamera::~fbcamera(){
  // allow the solving thread to rejoin.
  pthread_join(stack_and_save_thread,NULL);
  // deallocate local buffers
  free(camera_parameters.stackbuf);
  for(int idx=0;idx<IMAGE_BUFFERS;idx++){
    free(camera_parameters.picbuf[idx]);
  };
  if (camera_on){
    // deallocate buffers
    err = grabber.Free_Framebuffer();
    // stop recording
    err = camera.PCO_SetRecordingState(0);
    // close grabber
    err = grabber.Close_Grabber();
    // close camera
    err = camera.Close_Cam();
    DEBUGPRINT("Camera error: 0x%x\n",err);
    fblog::logmsg("Closed camera\n");
  } else {
    // camera in simulate mode
    nocamera::close();
  }; // camera_on
  
}; // fbcamera::~fbcamera();

// grab a single frame;
// returns NULL on failure;
int  fbcamera::grab_single(){
  static int bufidx;
  static int imgidx;
  
  // access the camera if the camerae is functioning
  if(camera_on && camera_open){
    picnum = 1;
    err = camera.PCO_SetRecordingState(1);
    err = grabber.Start_Aquire(picnum);
    err = grabber.Wait_For_Image(&picnum, timeout);
    err = grabber.Get_Framebuffer_adr(picnum, (void**)&adr);
    err = grabber.Stop_Aquire();


   
    grabber.Extract_Image(camera_parameters.picbuf[bufidx],
			  adr,
			  camera_parameters.width,
			  camera_parameters.height);
  } else {
    // do something else, otherwise. 
    nocamera::getimage(camera_parameters.picbuf[bufidx],
		       camera_parameters.exptime);
  };
  imgidx = bufidx;
  // cycle through available buffers
  bufidx = (bufidx+1) % IMAGE_BUFFERS;
  
  // make a note of current frame number, if rolling over.
  if( camera_parameters.framenumber % FRAMENUMBER_SAVE_FREQUENCY == 0)
    save_framenumber();
  // incrament framenumber;
  camera_parameters.framenumber++;
  
  return imgidx;
}; //fbcamera::grab_single;

// get the various temperatures from the camera
int fbcamera::get_temperature(){
  if ( camera_on ) { 
    err = camera.PCO_GetTemperature(&camera_parameters.ccdtemp,
				    &camera_parameters.camtemp,
				    &camera_parameters.pstemp);
    
    if (err!=0){
      fblog::logerr("Could not get temperature [0x%x]",err);
    } else {
      fblog::logmsg("Temps. CCD=%d, CAM=%d, PS=%d",
		    camera_parameters.ccdtemp,
		    camera_parameters.camtemp, 
		    camera_parameters.pstemp );
    };
  };		   
  return 0;
}; //fbcamera::get_temperature

uint32_t fbcamera::GetBinning(uint16_t *binningx, uint16_t *binningy){
  SC2_Simple_Telegram com;
  SC2_Binning_Response resp;

  com.wCode = GET_BINNING;
  com.wSize = sizeof(com);

  err = (camera).Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  if(err==PCO_NOERROR){
    *binningx = resp.wBinningx;
    *binningy = resp.wBinningy;
  };
  return err;  
}; //fbcamera::GetBinning

uint32_t  fbcamera::SetBinning(uint16_t binningx,
			       uint16_t binningy){

  SC2_Set_Binning com;
  SC2_Binning_Response resp;
  uint32_t err;

  com.wCode = SET_BINNING;
  com.wSize = sizeof(com);
  com.wBinningx = binningx;
  com.wBinningy = binningy;

  err = (camera).Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}; // fbcamera::SetBinning

uint32_t fbcamera::SetROI(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
  SC2_Set_ROI com;
  SC2_ROI_Response resp;
  uint32_t err;
  com.wCode = SET_ROI;
  com.wSize = sizeof(com);
  com.wROI_x0 = x0;
  com.wROI_y0 = y0;
  com.wROI_x1 = x1;
  com.wROI_y1 = y1;

  err = (camera).Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  
  return err;
}; //fbcamera::PCO_SetROI


int fbcamera::config_stack_and_solve(unsigned char stack, unsigned short stacksize, unsigned char solve){
  if(stacksize > 0) { 
    camera_parameters.stacking = FALSE;
    if(stack > 0) camera_parameters.stacking = TRUE;
    camera_parameters.solvestack = 0;
    if(solve == 1) camera_parameters.solvestack = 1;
    if(solve == 2) camera_parameters.solvestack = 2;
    // This is for periodic saving solve=10
    if(solve == 10) camera_parameters.solvestack = 10;
    if(stacksize < 100) { 
      camera_parameters.stack_size = stacksize;
    } else {
      camera_parameters.stack_size = 100;
    }; // stacksize < 100
  } else { 
    camera_parameters.stacking = FALSE;
  }; // stacksize
  return 0;
}; // config_stack_and_solve()

int fbcamera::stack_and_solve_image(uint16_t *buf){
  // only do something if stacking;
  if(camera_parameters.stacking){
    // is this the first instance? Clear the container if so
    if(camera_parameters.stack_count == 0){
      stack_structure.exptime = 0;
      stack_structure.count =0;
      // what is the astrometry mode?

      // store in img_save or img directory depending on whether periodic
      // or commanded, periodic saving is 10

      if (camera_parameters.solvestack == 10){
	stack_structure.astrometry_mode = 0;
	  sprintf(stack_structure.filename, 
		  "img_save/stack%07ld",camera_parameters.framenumber);

      } else {
	stack_structure.astrometry_mode = camera_parameters.solvestack;
	sprintf(stack_structure.filename, 
		"img/stack%07ld",camera_parameters.framenumber);
      }
      // populate the RA DEC here
      stack_structure.ra = 0.0;
      stack_structure.dec = 0.0;
      stack_structure.roll = 0.0;      
      fblog::logmsg("Starting stack: %s.",stack_structure.filename);
      stack_structure.buffsize=
	camera_parameters.width * camera_parameters.height;
      // fill out the ra and dec parameters here
      stack_structure.first_framenumber = camera_parameters.framenumber;
      // clear the memory
      memset(camera_parameters.stackbuf,0,stack_structure.buffsize*sizeof(float));
      // set the width and height
      stack_structure.width = camera_parameters.width;
      stack_structure.height = camera_parameters.height;
      stack_structure.buf = camera_parameters.stackbuf; 
    }; // first instance
  
    // now we just accumulate stuff
    for(int idx=0; idx<stack_structure.buffsize; idx++)
      *(camera_parameters.stackbuf+idx) += *(buf+idx);

    stack_structure.exptime += camera_parameters.exptime;
    stack_structure.count++;
    // is this the last instance? Spin off a thread that will save and
    // solve the field 
    camera_parameters.stack_count++;
    if(camera_parameters.stack_count >= camera_parameters.stack_size){
      stack_structure.last_framenumber = camera_parameters.framenumber;
      //spawn_thread that saves and then tries to solve the field
      pthread_create(&stack_and_save_thread, NULL,
		     stacking::stack_and_save_thread_worker,
		     (void*) &stack_structure);
      pthread_detach(stack_and_save_thread);
      // clean up
      camera_parameters.stack_count = 0;
      camera_parameters.stacking = FALSE; 
      camera_parameters.solvestack = 0;
    }; // last in the stack?
  }; // stacking?
  return 0;
}; // stack_image


int fbcamera::load_framenumber(){
  static FILE *fp;
  static long int fn;
  int res;
  fp = NULL;
  fp = fopen(FRAMENUMBER_FILENAME,"r");
  if(fp != NULL){
    res = fscanf(fp,"%ld\n",&fn);
    if(res != 1){
      camera_parameters.framenumber = 0;
      fblog::logerr("Failed to read in framenumber. Setting 0.");
    } else {
      camera_parameters.framenumber = fn+FRAMENUMBER_SAVE_FREQUENCY;
      fblog::logmsg("Loaded framenumber: %ld",camera_parameters.framenumber);
    };
    fclose(fp);

  } else {
    camera_parameters.framenumber = 0;
    fblog::logmsg("Assigned framenumber: 0");
  }; 
      return 0;
}; //fbcamera::load_framenumber

int fbcamera::save_framenumber(){
  static FILE *fp;
  fp = fopen(FRAMENUMBER_FILENAME,"w");
  fprintf(fp,"%ld\n",camera_parameters.framenumber);
  fclose(fp);
  return 0;
}; //fbcamera::save_framenumber


int fbcamera::set_exposure(uint32_t exposure){
  static uint32_t in_delay, in_exp;
  if ( camera_on ) { 
    if (exposure >= EXP_MIN && exposure <= EXP_MAX){
      err = camera.PCO_ArmCamera();
      err = camera.PCO_SetRecordingState(0);
      if (exposure < EXP_CUTOFF){
	delay = EXP_CUTOFF-exposure;
      } else {
	delay = 0;
      };
      err = camera.PCO_SetDelayExposure(delay, exposure);
      err = camera.PCO_GetDelayExposure(&in_delay,&in_exp);
      delay = in_delay;
      camera_parameters.exptime = in_exp;
      err = camera.PCO_ArmCamera();
      err = camera.PCO_SetRecordingState(1);
      fblog::logmsg("new (delay, exptime) = (%d, %d)",in_delay, in_exp);
    } else { 
      return -1;
    };
  } else { 
    camera_parameters.exptime = in_exp;
    nocamera::set_exptime(in_exp);
  };
  return 0;
}; // fbcamera::set_exposure();


void fbcamera::set_camera_on(char camon){
  printf("setting camera on to %d\n",camon);
  camera_on = camon;
};

char fbcamera::get_camera_on(){
  return camera_on;
};

int request_new_exptime(uint32_t exptime){
  new_exptime.exptime = exptime;
  new_exptime.flag = TRUE;
  fblog::logmsg("New exptime req: %d ms", exptime);
  return 0;
}; // request_new_exptime

int update_exptime(fbcamera *fbcam){
  if(new_exptime.flag){
    new_exptime.flag = FALSE;
    if(new_exptime.exptime != fbcam->camera_parameters.exptime)
      return fbcam->set_exposure(new_exptime.exptime);
  };
  return 0;
}; //update_exptime


int request_new_stack(unsigned char stack,
		      unsigned short stacksize,
		      unsigned char solve){
  new_stack.stack = stack;
  new_stack.solve = solve;
  new_stack.stacksize = stacksize;
  new_stack.flag = TRUE;
  fblog::logmsg("New stack req: %d %d %d.",stack,stacksize,solve);
  return 0;
}; // request_new_stack

int update_stack(fbcamera *fbcam){
  if(new_stack.flag){
    new_stack.flag = FALSE;
    return fbcam->config_stack_and_solve(new_stack.stack, 
					 new_stack.stacksize, 
					 new_stack.solve);
  };
  return 0;
}; //update_stack


// function to access temperatures
void retrieve_temperatures(short *ccdtemp,
			   short *camtemp,
			   short *pstemp){

  *ccdtemp = (fbcamera_parameters->ccdtemp);
  *camtemp = (fbcamera_parameters->camtemp);
  *pstemp = (fbcamera_parameters->pstemp);
  return;
};// retrieve_temperatures

//function to retrieve exposure time
void retrieve_exptime(int *exptime){
  *exptime = (fbcamera_parameters->exptime);
  return;
}; //retrieve exptime
