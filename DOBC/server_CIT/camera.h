/*
  This header file contains the structure definitions and function
  declarations needed to operate the EMCCD with the NUVU
  controller. It is based on Nov 2015.
*/

#ifndef CAMERA_H_
#define CAMERA_H_

#define SIMULATE 0

// legact from NUVU software stuff
#define COMMAND_LINE_BUILD 1

#define STRING_LENGTH 255

// structure for camera parameters
typedef struct NcPtcArgs {
  // nuvu params
  int emgain;
  float preamp;
  int vss;
  int cdsoffset;
  int imagenum;
  float exptime;
  int unit;
  int channel;
  int nbias;
  int maxFiles;
  int xsize;
  int ysize;
  double waitingTime;
  int readoutSeq;
  int ROS;
  //char shutter[1024];
  int compress;
  char *extraOpt;
  int shutter;
  int trigger;
  int extshutter;
  int shutterdelay;
  // additional params
  char image_path[STRING_LENGTH];
  char root_name[STRING_LENGTH];
  char last_fname[STRING_LENGTH];
  int imagenumber;
  int imageburst;
  char simulate;
}NcPtcArgs;


//  open and close functions
int open_camera(int ROS);
int close_camera();

// set various parameters functions
int set_emgain(int emgain); // done
int set_vss(int vss); // done
int set_cdsoffset(int cdsoffset); // done
int set_exposure_time(float exptime); // done
int set_shutterdelay(int shutterdelay); // done
int set_exposure_burst(int burst); //done
int set_xsize(int number); // done
int set_ysize(int number); // done
int set_exposure_number(int number); // done
int set_shutter(int setting); 
int set_trigger(int trigger);
int set_extshutter(int extshutter);
int set_imagename_root(char *root);
int set_imagename_path(char *path);


// get various parameters functions
int get_emgain(int *emgain); //done
int get_vss(int *vss); //done
int get_cdsoffset(int *cdsoffset); //done
int get_exposure_time(float *exptime); // done
int get_shutterdelay(int *shutterdelay); // done
int get_exposure_burst(int *burst); //done
int get_xsize(int *number); // done
int get_ysize(int *number); // done
int get_exposure_number(int *number); // done
int get_shutter(int *setting);
int get_trigger(int *trigger);
int get_extshutter(int *extshutter);
int get_imagename_root(char *root);
int get_imagename_path(char *path);
int get_last_filename(char *fname);
int get_next_filename(char *fname);

// expose!
int expose();
int dark();
int bias();
int close_shutter();
int open_shutter();

// **** 161102 START
// add_header
int add_header(char *fname);
int add_temps(char *dat, char*tim, char *celsius1, char *celsius2);
// **** 161102 END



#endif // CAMERA_H_
