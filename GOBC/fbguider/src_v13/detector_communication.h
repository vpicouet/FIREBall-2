#ifndef DETECTOR_COMMUNICATION_H_
#define DETECTOR_COMMUNICATION_H_

/* things we will send */
/* 
 EXP=1 -- command to take an exposure. The control of the exposure
 (exposure time and gain) rest with the detector operator. 

 SAV=filename -- the name of the last filename saved on guider

 STG=comma separated stage positions

 GON=Azimuth, Elevation, Rotation, RA, Dec, Roll

 LAM=Lamp and shutter bits, comma separated

 TAG=updates the tag. 
*/

  #define DET_EXPOSE_TLC "EXP"
 #define DET_SAVED_TLC "SAV"
  #define DET_STAGE_TLC "STG"
  #define DET_GONDOLA_TLC "GON"
  #define DET_LAMP_TLC "LAM"
 #define DET_TEXT_TLC "TXT"
  #define DET_TAG_TLC "TAG"


namespace detector{

  void toggle_detector_command(char stat);
  char get_detector_command();
  
  void send_tlc(const char[], char *buffer);
  void send_saveframe();
  void send_stageinfo();
  void send_gondolainfo();
  void send_lampinfo();
  void send_tag();
  void send_text(char *buffer);
  void command_exposure(char *infobuffer);
};
#endif 
