//CIT V3 controller

// standard C/C++ stuff
#include <cstdio>
#include <string>
#include <cstdlib>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>

// nuvu specific stuff
#include <nc_driver.h>

// camera declarations
#include "camera.h"
#include "fitsio.h"

// globals
NcPtcArgs params;
NcGrab grab = NULL;
unsigned char grab_open=0;
int err=0;

int open_camera(int ROS){
  int n;
  char temp[1000];
  //char *const serialNumber[256];
  
  // populate the parameters
  params.simulate = SIMULATE;
  // nuvu/edt stuff
  params.unit = EDT_CL;
  params.channel = 0;
  params.compress = 0;
  params.nbias = -1;
  params.xsize = 3216;
  params.ysize = 2069;
  params.waitingTime = 0.0;
  params.readoutSeq = ROS;
  params.maxFiles = 10000;
  params.extraOpt = NULL;
  params.shutter = 0;
  params.trigger = 0;
  params.extshutter = 1; //set to 1 for when an external shutter is present
  params.shutterdelay = 100;
  params.emgain = 0;
  params.preamp = 0;
  params.vss = 0;
  params.cdsoffset = 500;
  params.exptime = 0;
  // additional params
  sprintf(params.image_path,"./");
  sprintf(params.root_name,"image");
  sprintf(params.last_fname," ");
  params.imagenumber=0;
  params.imageburst=1;

  // if you wanted to load new parameters, this would be the place to
  // do so, and update everything in `params'

  if(!params.simulate){
    // open the grabber
    if((err = ncGrabOpen(params.unit, params.channel, 32, &grab))) {
      printf("Unable to open grab: %d\n", err);
      return err;
    }; //ncGrabOpen
    printf("Camera opened.\n");

    ////request serial number
    //if((err = ncGrabGetControllerSerialNumber(0, *serialNumber))) {
    //  printf("Unable to read seial number from controller: %d\n", err);
    //  return err;
    //}; //
    //printf("Serial Number: %s\n", serialNumber);

    // set strobesel, this is important for V2
    //n = sprintf(temp, "strobesel 0\n");
    //if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
    //  printf("Unable to set strobesel 0: %d\n", err);
    //  close_camera();
    //  return err;
    //}; // BinaryComm Strobesel
    //printf("Strobesel set to 0\n");

    // set the readout sequence
    if ( params.readoutSeq >= 0) { 
      n = sprintf(temp, "ld %d\n", params.readoutSeq);
      if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
	printf("Unable to set Readout Sequence: %d\n", err);
	close_camera();
	return err;
      }; //BinaryComm readout sequence
      printf("Readout Sequence set to %d\n", params.readoutSeq);
    } else {
      printf("Readout sequence not set!!!\n");
    };
  
    // Set strobesel, this is important for V2
    //n = sprintf(temp, "strobesel 1\n");    
    //if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
    //  printf("Unable to set strobesel 1: %d\n", err);
    //  close_camera();
    //  return err;
    //}; // BinaryComm(strobesel)
    //printf("Strobesel set to 1\n");

    // set the shutter mode
    n = sprintf(temp, "sesm %d\n", params.shutter);
    if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
      printf("Unable to set shutter sequence: %d\n", err);
      close_camera();
      return err;
    }; // BinaryComm(shutter);
    printf("Shutter mode set to %d\n", params.shutter);


    n = sprintf(temp, "ssp -1\n");
    if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
      printf("Unable to set shutter polarity: %d\n", err);
      close_camera();
      return err;
    }; // BinaryComm(shutter);
    printf("Shutter polarity set to -1\n");


    // presence of external shutter
    n = sprintf(temp, "sesp %d\n", params.extshutter);
    if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
      printf("Unable to set external shutter: %d\n", err);
      close_camera();
      return err;
    }; // BinaryComm(extshutter);
    printf("External shutter mode set to %d\n", params.extshutter);

    // set the shutter trigger
    n = sprintf(temp, "stm %d\n", params.trigger);
    if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
      printf("Unable to set shutter trigger sequence: %d\n", err);
      close_camera();
      return err;
    }; // BinaryComm(shutter);
    printf("Shutter trigger mode set to %d\n", params.trigger);

    // image size for the grabber
    const int width = 3216;
    const int height = 2069;
    if((err =ncGrabSetSize(grab, width, height))){
      printf("Unable to set Grab Size. err=%d\n",err);
      close_camera();
      return err;
    }; // grabsize();
    printf("Grab size set to %d x %d\n",params.xsize,params.ysize);
    
    // grab set timeout
    if((err = ncGrabSetTimeout(grab, 0))) {
      printf("Unable to set timeout: %d\n", err);
      close_camera();
      return err;
    }; //grabsettimeout
    printf("Grab set timeout.\n");

    // set the waiting time 
    n = sprintf(temp, "sw %f\n", params.waitingTime);
    if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
      printf("Unable to set waiting time: %d\n", err);
      close_camera();
      return err;
    }; // waitingtime
    printf("Waiting time set to %f\n", params.waitingTime);

    grab_open = 1;
    set_exposure_time(params.exptime);
    set_shutterdelay(params.shutterdelay);
    //set_extshutter(params.extshutter);
    set_emgain(params.emgain);
    set_vss(params.vss);
    set_cdsoffset(params.cdsoffset);

    
  } else { 
    printf("[SIM] Camera opened.\n");
    grab_open = 1;
  }; //params_mode

  return 0;
}; //open_camera();

int close_camera(){
  //int n;
  //char temp[255];
  if(!params.simulate){
    if(grab){

      //n = sprintf(temp, "abort\n");
      //if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
	//printf("Unable to abort ROS: %d\n", err);
      //}; //BinaryComm readout sequence
      //printf("ROS aborted.\n");

      //n = sprintf(temp, "erasesequence\n");
      //if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
	//printf("Unable to erase sequence: %d\n", err);
      //}; //BinaryComm readout sequence
      //printf("FPGA Sequence erased.\n");

      ncGrabClose(grab);
      grab=NULL;
      printf("Closed camera\n");
      grab_open=0;
    };
  } else {
    printf("[SIM] Closed camera.\n");
    grab_open=0;
  };
  return 0;
};

int set_xsize(int number){
  if(grab_open){
    if(number>=0){
      params.xsize = number;
    } else {
      printf("Invalid xsize for array %d\n",number);
      return -1;
    };    
  } else { 
    printf("Camera not open.\n");
    return -1;
  }; 
  return 0;
}; // set_xsize

int get_xsize(int *number){
  if(grab_open){
    *number = params.xsize;
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // get_xsize

int set_ysize(int number){
  if(grab_open){
    if(number>=0){
      params.ysize = number;
    } else {
      printf("Invalid ysize for array %d\n",number);
      return -1;
    };    
  } else { 
    printf("Camera not open.\n");
    return -1;
  }; 
  return 0;
}; // set_ysize

int get_ysize(int *number){
  if(grab_open){
    *number = params.ysize;
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // get_ysize

int set_shutterdelay(int shutterdelay){
  static int n;
  static char temp[256];
  err = 0;
  if(grab_open){
    if(!params.simulate){
      // MM -- ideally there would be some error checking on the range
      // here
      if (shutterdelay >= 0){
	if((err = ncGrabSetTimeout(grab, (int)(shutterdelay)))) {
	  printf("Unable to set timeout: %d\n", err);
	  return err;
	}; // grab time out
	// Set the exposing time
	n = sprintf(temp, "ssd %d\n", (shutterdelay));
	if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
	  printf("Unable to set shutter delay: %d\n", err);
	  return err;
	}; //
	params.shutterdelay = shutterdelay;
	printf("Set shutter delay to %d\n",(shutterdelay));
      } else {
	printf("Shutter delay out of range\n");
	return -1;
      };
    } else { 
      params.shutterdelay = shutterdelay;
      printf("[SIM] Set shutter delay to %d\n",(shutterdelay));
    };
  } else {
    printf("Camera not open!?.\n");
    return -1;
  };
  return 0;
}; //set_shutterdelay();

int get_shutterdelay(int *val){
  if(grab_open){
    if(!params.simulate){
      // in the future we might poll the camera here
      *val = params.shutterdelay;
    } else {
      *val = params.shutterdelay;
      printf("[SIM] Got shutter delay\n");
    };
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // get_shutterdelay

int set_exposure_time(float exptime){
  static int n;
  static char temp[256];
  err = 0;
  if(grab_open){
    if(!params.simulate){
      // MM -- ideally there would be some error checking on the range
      // here
      if (exptime >= 0){
	if((err = ncGrabSetTimeout(grab, (float)(exptime+10)*1000))) {
	  printf("Unable to set timeout: %d\n", err);
	  return err;
	}; // grab time out
	// Set the exposing time
	n = sprintf(temp, "se %f\n", exptime*1000.0);
	if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
	  printf("Unable to set exposing time: %d\n", err);
	  return err;
	}; //
	params.exptime = exptime;
	printf("Set exptime to %0.2f\n",exptime);
      } else {
	printf("EXPTIME out of range\n");
	return -1;
      };
    } else { 
      params.exptime = exptime;
      printf("[SIM] Set exptime to %0.2f\n",exptime);
    };
  } else {
    printf("Camera not open!?.\n");
    return -1;
  };
  return 0;
}; //set_exposure_time();

int get_exposure_time(float *val){
  if(grab_open){
    if(!params.simulate){
      // in the future we might poll the camera here
      *val = params.exptime;
    } else {
      *val = params.exptime;
      printf("[SIM] Got exposure time\n");
    };
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // get_exposure_time

int set_emgain(int emgain){
  int n;
  char temp[256];
  if(grab_open){
    if(!params.simulate){
      // Set the emgain value
      printf("EM gain: %d\n", emgain);
      n = sprintf(temp, "sb 11 %d\n", emgain);
      if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
      printf("Unable to set EM gain: %d\n", err);
      return -1;
      }; //err
      params.emgain = emgain;
      printf("Set emgain to %d\n",emgain);
    } else {
      params.emgain = emgain;
      printf("[SIM] Set emgain to %d\n",emgain);
    };
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // set_emgain();


int get_emgain(int *val){
  if(grab_open){
    if(!params.simulate){
      // in the future we might poll the camera here
      *val = params.emgain;
    } else {
      *val = params.emgain;
    };
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // get_emgain();

int set_vss(int vss){
  int n;
  int vss_array[9] = {0, 1631, 3269, 4907, 6548, 8183, 9822, 11460, 13098};
  char temp[256];
  if(grab_open){
    if(!params.simulate){
      // Set the vss value
      //printf("VSS: %d\n", vss_array[vss]);
      n = sprintf(temp, "sb 15 %d\n", vss_array[vss]);
      if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
      printf("Unable to set VSS: %d\n", err);
      return -1;
      }; //err
      params.vss = vss;
      printf("Set VSS to %d\n",vss);
    } else {
      params.vss = vss;
      printf("[SIM] Set vss to %d\n",vss);
    };
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // set_vss();

int get_vss(int *val){
  if(grab_open){
    if(!params.simulate){
      // in the future we might poll the camera here
      *val = params.vss;
    } else {
      *val = params.vss;
    };
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // get_vss();

int set_cdsoffset(int cdsoffset){
  int n;
  char temp[256];
  //printf("Passed: %d\n",cdsoffset);
  if(grab_open){
    if(!params.simulate){
      // Set the cdsoffset value
      printf("cdsoffset: %d\n", cdsoffset);
      n = sprintf(temp, "cdsoffset %d\n", cdsoffset);
      if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
      printf("Unable to set cdsoffset: %d\n", err);
      return -1;
      }; //err
      params.cdsoffset = cdsoffset;
      printf("Set cdsoffset to %d\n",cdsoffset);
    } else {
      params.cdsoffset = cdsoffset;
      printf("[SIM] Set cdsoffset to %d\n",cdsoffset);
    };
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // set_cdsoffset();

int get_cdsoffset(int *val){
  if(grab_open){
    if(!params.simulate){
      // in the future we might poll the camera here
      *val = params.cdsoffset;
    } else {
      *val = params.cdsoffset;
    };
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // get_cdsoffset();

int set_exposure_burst(int burst){
  if(grab_open){
    if(burst>0){
      params.imageburst = burst;
    } else {
      printf("Invalid burst size: %d\n",burst);
      return -1;
    };
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // set_exposure_burst();

int get_exposure_burst(int *burst){
  if(grab_open){
    *burst = params.imageburst;
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // get_exposure_burst();

int set_exposure_number(int number){
  if(grab_open){
    if(number>=0){
      params.imagenumber = number;
    } else {
      printf("Invalid exposure number %d\n",number);
      return -1;
    };
  } else { 
    printf("Camera not open.\n");
    return -1;
  }; 
  return 0;
}; // set_exposure_number

int get_exposure_number(int *number){
  if(grab_open){
    *number = params.imagenumber;
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // get_exposure_number


int set_imagename_root(char *root){
  if (grab_open){
    sprintf(params.root_name,"%s",root);
  } else {
    printf("Camera not open\n");
    return -1;
  };
  return 0;
}; // set_imagename_root

int get_imagename_root(char *root){
  return sprintf(root,"%s",params.root_name);
}; // get_imagename_root

int set_imagename_path(char *path){
  if (grab_open){
    sprintf(params.image_path,"%s/",path);
  } else { 
    printf("Camera not open\n");
    return -1;
  };
  return 0;
}; // set_imagename_path

int get_imagename_path(char *path){
  return sprintf(path, "%s",params.image_path);
  
}; // get_imagename_path

int set_shutter(int setting){
  int n;
  char temp[256];
  if(setting <-2 || setting > 2){
    printf("Invalid shutter setting (%d)\n",setting);
    return -1;
  } else {
    if(grab_open){
      if(!params.simulate){
	n = sprintf(temp, "sesm %d\n", setting);
        usleep(1000);
	if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
	  printf("Unable to set shutter sequence: %d\n", err);
	  return -1;
	}; // BinaryComm(shutter);
	//params.shutter = setting;
	printf("Shutter mode set to %d\n", setting);
        usleep(1000);
	return 0;
      } else {
	// in simulate, we just set the value...
	//params.shutter = setting;
	printf("[SIM] Set shutter to: %d\n",setting);
      };
    } else {
      printf("Camera not open\n");
      return -1;
    };
  };
  return 0;
}; // set_shutter

int get_shutter(int *setting){
  if(grab_open){
    if(!params.simulate){
      // in the future we may actually ask the camera
      *setting = params.shutter;
      return 0;
    } else {
      // in simulate, we just return the value...
      *setting = params.shutter;
      return 0;
    };
  } else {
    printf("Camera not open\n");
    return -1;
  };
  return 0;
};// get shutter

int set_trigger(int trigger){
  int n;
  char temp[256];
  //printf("Passed: %d\n",cdsoffset);
  if(grab_open){
    if(!params.simulate){
      // Set the trigger value
      printf("stm: %d\n", trigger);
      n = sprintf(temp, "stm %d\n", trigger);
      if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
      printf("Unable to set trigger: %d\n", err);
      return -1;
      }; //err
      params.trigger = trigger;
      printf("Set trigger to %d\n",trigger);
    } else {
      params.trigger = trigger;
      printf("[SIM] Set trigger to %d\n",trigger);
    };
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // set_trigger();

int get_extshutter(int *val){
  if(grab_open){
    if(!params.simulate){
      // in the future we might poll the camera here
      *val = params.extshutter;
    } else {
      *val = params.extshutter;
    };
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // get_extshutter();

int set_extshutter(int extshutter){
  int n;
  char temp[256];
  //printf("Passed: %d\n",cdsoffset);
  if(grab_open){
    if(!params.simulate){
      // Set the trigger value
      printf("sesp: %d\n", extshutter);
      n = sprintf(temp, "stm %d\n", extshutter);
      if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
      printf("Unable to set extshutter: %d\n", err);
      return -1;
      }; //err
      params.extshutter = extshutter;
      printf("Set extshutter to %d\n",extshutter);
    } else {
      params.extshutter = extshutter;
      printf("[SIM] Set extshutter to %d\n",extshutter);
    };
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // set_extshutter();

int get_trigger(int *val){
  if(grab_open){
    if(!params.simulate){
      // in the future we might poll the camera here
      *val = params.trigger;
    } else {
      *val = params.trigger;
    };
  } else {
    printf("Camera not open.\n");
    return -1;
  };
  return 0;
}; // get_trigger();

int get_next_filename(char *fname){
  if(grab_open){
    return sprintf(fname,"%s%s%06d.fits",params.image_path,params.root_name,params.imagenumber);
  } else {
    printf("Camera not open\n");
    return -1;
  };
  return 0;
}; // get_next_filename

int get_last_filename(char *fname){
  if(grab_open){
    return sprintf(fname,"%s",params.last_fname);
  } else {
    printf("Camera not open\n");
    return -1;
  };
  return 0;
}; // get_last_filename


int expose(){
  int idx,n, nTimeout;
  const int width = params.xsize;
  const int height = params.ysize;
  int timeout1, timeout2;
  char temp[1000];
  nTimeout=0;
  set_shutter(0);
  if(grab_open){
    if(!params.simulate){
      // flush read queue
      ncGrabFlushReadQueues(grab);
      // get the first timeout info
      ncGrabGetNbrTimeout(grab, &timeout1);
      // image size for the grabber
      if((err =ncGrabSetSize(grab, width, height))){
        printf("Unable to set Grab Size. err=%d\n",err);
        //close_camera();
        return err;
        }; // grabsize();
      printf("Grab size set to %d x %d\n",params.xsize,params.ysize);

      // start grab for desired number of images
      ncGrabStart(grab, params.imageburst);
      n = sprintf(temp,"re %d\n",params.imageburst);
      if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
	printf("Unable to request exposures: %d\n", err);
	n = sprintf(temp, "sb 11 0\n");
	if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
          printf("Unable to set EM gain: %d\n", err);
	}
	ncGrabAbort(grab);
	return -1;
      };
      for(idx=0;idx<params.imageburst; idx++){
	double time;
	NcImage *image;
	if((err = ncGrabReadTimed(grab, &image, &time))) {
	  printf("Error reading image: %d\n", err);
	}
	ncGrabGetNbrTimeout(grab, &timeout2);
	if(timeout1 != timeout2) {
	  nTimeout++;
	  if(nTimeout >= 10) {
	    printf("Failed to take image.\n");
	    return -1;
	  }
	} else {
	  sprintf(temp, "%s%s%06d", params.image_path, params.root_name, params.imagenumber);
	  nTimeout=0;
	  printf("Recording %s%06d.fits\n", params.root_name, params.imagenumber);
	  ncSaveImage(params.xsize, params.ysize, NULL, image, NC_IMG, temp, FITS, NO_COMPRESSION, "", 0);
	  //ncGrabSaveImage(grab, image, temp, FITS, -1);
	  sprintf(params.last_fname,"%s",temp);
          usleep(1000*1000);	  

	  // **** 161102 START
	  // Update image headers using cfitsio 
	  sprintf(temp,"%s.fits",params.last_fname);
	  add_header(temp);
	  // **** 161102 END	  
	  // increment image number
	  params.imagenumber++;
	};
	timeout1 = timeout2;

	fflush(stdout);
      }; // idx loop

      // Set the emgain value
      printf("EM gain: 0\n");
      n = sprintf(temp, "sb 11 0\n");
      if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
          printf("Unable to set EM gain: %d\n", err);
      }

      ncGrabAbort(grab);

    } else {
      // simulate
      for(idx=0;idx<params.imageburst;idx++){
	usleep(params.exptime*1000);
	printf("Simulate:  image %d (%d of %d), exptime=%0.2d ms\n",
	       params.imagenumber,
	       idx+1,params.imageburst, 
	       params.exptime);
	sprintf(params.last_fname, "%s%s%06d.fits", params.image_path, params.root_name, params.imagenumber);
	params.imagenumber++;

      };
    };
  } else {
    printf("Camera not open\n");
    return -1;
  };
  return 0;
};


int dark(){
  int idx,n, nTimeout;
  int timeout1, timeout2;
  char temp[1000];
  nTimeout=0;
  set_shutter(2);
  if(grab_open){
    if(!params.simulate){
      // flush read queue
      ncGrabFlushReadQueues(grab);
      // get the first timeout info
      ncGrabGetNbrTimeout(grab, &timeout1);
      // start grab for desired number of images
      ncGrabStart(grab, params.imageburst);
      n = sprintf(temp,"re %d\n",params.imageburst);
      if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
	printf("Unable to request exposures: %d\n", err);
      n = sprintf(temp, "sb 11 0\n");
      if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
          printf("Unable to set EM gain: %d\n", err);
      }
  ncGrabAbort(grab);
	return -1;
      };
      for(idx=0;idx<params.imageburst; idx++){
	double time;
	NcImage *image;
	if((err = ncGrabReadTimed(grab, &image, &time))) {
	  printf("Error reading image: %d\n", err);
	}
	ncGrabGetNbrTimeout(grab, &timeout2);
	if(timeout1 != timeout2) {
	  nTimeout++;
	  if(nTimeout >= 10) {
	    printf("Failed to take image.\n");
	    return -1;
	  }
	} else {
	  sprintf(temp, "%s%s%06d", params.image_path, params.root_name, params.imagenumber);
	  nTimeout=0;
	  printf("Recording %s%06d.fits\n", params.root_name, params.imagenumber);
	  ncSaveImage(params.xsize, params.ysize, NULL, image, NC_IMG, temp, FITS, NO_COMPRESSION, "", 0);
	  sprintf(params.last_fname,"%s",temp);
	  // **** 161102 START
	  // Update image headers using cfitsio 
	  sprintf(temp,"%s.fits",params.last_fname);
	  add_header(temp);
	  // **** 161102 END
	  // increment image number
	  params.imagenumber++;
	};
	timeout1 = timeout2;

	fflush(stdout);
      }; // idx loop

      // Set the emgain value
      printf("EM gain: 0\n");
      n = sprintf(temp, "sb 11 0\n");
      if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
          printf("Unable to set EM gain: %d\n", err);
      }

      ncGrabAbort(grab);

    } else {
      // simulate
      for(idx=0;idx<params.imageburst;idx++){
	usleep(params.exptime*1000);
	printf("Simulate:  image %d (%d of %d), exptime=%d ms\n",
	       params.imagenumber,
	       idx+1,params.imageburst, 
	       params.exptime);
	sprintf(params.last_fname, "%s%s%06d.fits", params.image_path, params.root_name, params.imagenumber);
	params.imagenumber++;

      };
    };
  } else {
    printf("Camera not open\n");
    return -1;
  };
  return 0;
};

int bias(){
  int idx,n, nTimeout;
  int timeout1, timeout2;
  char temp[1000];
  nTimeout=0;
  set_shutter(2);
  set_exposure_time(0);
  if(grab_open){
    if(!params.simulate){
      // flush read queue
      ncGrabFlushReadQueues(grab);
      // get the first timeout info
      ncGrabGetNbrTimeout(grab, &timeout1);
      // start grab for desired number of images
      ncGrabStart(grab, params.imageburst);
      n = sprintf(temp,"re %d\n",params.imageburst);
      if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
	printf("Unable to request exposures: %d\n", err);
      n = sprintf(temp, "sb 11 0\n");
      if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
          printf("Unable to set EM gain: %d\n", err);
      }
  ncGrabAbort(grab);
	return -1;
      };
      for(idx=0;idx<params.imageburst; idx++){
	double time;
	NcImage *image;
	if((err = ncGrabReadTimed(grab, &image, &time))) {
	  printf("Error reading image: %d\n", err);
	}
	ncGrabGetNbrTimeout(grab, &timeout2);
	if(timeout1 != timeout2) {
	  nTimeout++;
	  if(nTimeout >= 10) {
	    printf("Failed to take image.\n");
	    return -1;
	  }
	} else {
	  sprintf(temp, "%s%s%06d", params.image_path, params.root_name, params.imagenumber);
	  nTimeout=0;
	  printf("Recording %s%06d.fits\n", params.root_name, params.imagenumber);
	  ncSaveImage(params.xsize, params.ysize, NULL, image, NC_IMG, temp, FITS, NO_COMPRESSION, "", 0);
	  sprintf(params.last_fname,"%s",temp);
	  // **** 161102 START
	  // Update image headers using cfitsio 
	  sprintf(temp,"%s.fits",params.last_fname);
	  add_header(temp);
	  // **** 161102 END
	  // increment image number
	  params.imagenumber++;
	};
	timeout1 = timeout2;

	fflush(stdout);
      }; // idx loop

      // Set the emgain value
      printf("EM gain: 0\n");
      n = sprintf(temp, "sb 11 0\n");
      if((err = ncGrabSendSerialBinaryComm(grab, temp, n))) {
          printf("Unable to set EM gain: %d\n", err);
      }

      ncGrabAbort(grab);

    } else {
      // simulate
      for(idx=0;idx<params.imageburst;idx++){
	usleep(params.exptime*1000);
	printf("Simulate:  image %d (%d of %d), exptime=%d ms\n",
	       params.imagenumber,
	       idx+1,params.imageburst, 
	       params.exptime);
	sprintf(params.last_fname, "%s%s%06d.fits", params.image_path, params.root_name, params.imagenumber);
	params.imagenumber++;

      };
    };
  } else {
    printf("Camera not open\n");
    return -1;
  };
  return 0;
};

int close_shutter(){
  int idx,n, nTimeout;
  int timeout1, timeout2;
  char temp[1000];
  nTimeout=0;
  set_shutter(2);
};

int open_shutter(){
  int idx,n, nTimeout;
  int timeout1, timeout2;
  char temp[1000];
  nTimeout=0;
  set_shutter(-2);
};

/* added 161102 to implement basic headers */
// **** 161102 START
int add_header(char *fname){
  // things for handling the fits file
  fitsfile *fptr;
  int iomode, status=0;
  char dat[255],tim[255],temp[255],camtemp1[255],camtemp2[255];
  iomode=READWRITE;
  
  if(! fits_open_file(&fptr, fname, iomode, &status) ){

    /* Update/Add Keywords */

    fits_write_date(fptr,&status);
    fits_update_key(fptr, TFLOAT, "EXPTIME",&params.exptime,"Exposure time (s)",&status);
    
    fits_update_key(fptr, TINT, "EMGAIN",&params.emgain,"EM Gain",&status);
    fits_update_key(fptr, TFLOAT, "GAIN1",&params.preamp,"Pre-amplifier gain",&status);
    fits_update_key(fptr, TINT, "VSS",&params.vss,"Substrate voltage",&status);
    fits_update_key(fptr, TINT, "IMNO",&params.imagenumber,"Image number",&status);
    fits_update_key(fptr, TINT, "IMBURST",&params.imageburst,"Image burst",&status);
    fits_update_key(fptr, TINT, "SHUTTER",&params.shutter,"Shutter state",&status);
    fits_update_key(fptr,TINT,"ROS",&params.readoutSeq,"Read-out sequence",&status);
    fits_update_key(fptr,TINT,"CDSOFFSET",&params.cdsoffset,"CDS Offset",&status);

    if ( ! add_temps(dat,tim,camtemp1,camtemp2) ) { 
      fits_update_key(fptr,TSTRING,"TEMPDATE",dat,"Temperature reading date",&status);
      fits_update_key(fptr,TSTRING,"TEMPTIME",tim,"Temperature reading time",&status);
      fits_update_key(fptr,TSTRING,"TEMPA",camtemp1,"Temperature reading (K)",&status);
      fits_update_key(fptr,TSTRING,"TEMPB",camtemp2,"Temperature reading (K)",&status);
    } else {
      printf("Couldn't add temperatures.\n");
    };

    // observation type
    switch (params.shutter){
    case 0:
      sprintf(temp,"OBJECT");
      break;
    case -2:
      if ( params.exptime > 0) {
	sprintf(temp,"DARK");
      } else {
	sprintf(temp,"BIAS");
      }
      break;
    default:
      sprintf(temp,"UNKNOWN");
      break;
    }
    fits_update_key(fptr,TSTRING,"OBSTYPE",temp,"Image type",&status);


    fits_close_file(fptr, &status);
    if ( status ) { 
      printf("Trouble updating FITS file: %s\n",fname);
      return -1;
    };
		    
  } else {
    printf("Could not open FITS file: %s\n",fname);
    return -1;
  };

  return 0;
};
// **** 161102 END

// **** 161102 START
int add_temps(char *dat, char *tim, char *celsius1, char *celsius2){
  FILE *fp;
  char path[1035];
  int ctr=0;
  fp = popen("/usr/local/bin/gettemp.sh","r");
  if (fp == NULL){
    printf("Could not get temperature.\n");
    return -1;
  } 
  
  while (fgets(path, sizeof(path)-1, fp) != NULL) {
    if ( ctr > 3) {
      printf("File format invalid.");
      return -2;
    }
    if (ctr == 0) sprintf(dat,"%s",path);
    if (ctr == 1) sprintf(tim,"%s",path);
    if (ctr == 2) sprintf(celsius1,"%s",path);
    if (ctr == 3) sprintf(celsius2,"%s",path);
    ctr++;
  }
  
  /* close */
  pclose(fp);


  return 0;
};
// **** 161102 END
