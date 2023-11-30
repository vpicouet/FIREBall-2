
#include <cstdio>

#include "detector_communication.h"
#include "communication.h"

#include "gondola_communication.h"
#include "detector_communication.h"
#include "adio.h"
#include "ground_communication.h"
#include "motors.h"

namespace detector{

  char command_detector; 
  char cmdmsg[255];

  void toggle_detector_command(char cmd){
    if(cmd){
      command_detector=1;
    } else {
      command_detector=0;
    };
  }; // toggle_detector_command

  char get_detector_command(){
    return command_detector;
  };


  void send_tlc(const char tlc[], char *buffer){
    static char msg[255];
    static int len;
    len=sprintf(msg,"%s=%s",tlc, buffer);
    if (len>0)
      write_to_detector(msg,len);    
    return;
  }; // send_tlc

  void send_saveframe(){
    
    send_tlc(DET_SAVED_TLC,cmdmsg);
    return;
  }; // send_saveframe()
  
  void send_stageinfo(){
    static float pos[4];
    motors::get_motor_positions(pos);
    sprintf(cmdmsg,"%.3f, %.3f, %.3f, %.3f",pos[0],pos[1],pos[2],pos[3]);
    send_tlc(DET_STAGE_TLC,cmdmsg);
    return;
  }; //send_stageinfo()

  void send_gondolainfo(){
    static double ra, dec, roll;
    static float az, el, rot;

    gondola::get_gondola_boresight(&az, &el, &rot);
    gondola::get_gondola_radec(&ra, &dec, &roll);
    sprintf(cmdmsg,"%.4lf, %.4lf, %.4lf, %.4lf, %.4lf, %.4lf",ra,dec,roll,az,el,rot);
    send_tlc(DET_GONDOLA_TLC,cmdmsg);
    return;
  }; //send_gondolainfo()
    
  void send_lampinfo(){
    static unsigned char lamps;
    adio::digital_in(&lamps);
    sprintf(cmdmsg,"%d",lamps);
    send_tlc(DET_LAMP_TLC,cmdmsg);
    return;
  }; // send_lampinfo();

  void send_tag(){
    static unsigned short imagetag;
    imagetag=img::get_image_tag();
    sprintf(cmdmsg,"%3d",imagetag);
    send_tlc(DET_TAG_TLC,cmdmsg);
    return;
  }; //send_tag();

  void send_text(char *buffer){
    send_tlc(DET_TEXT_TLC,buffer);
    return;
  }; //send_text()
  
  void command_exposure(char *info){
    if (command_detector) { 
      sprintf(cmdmsg,"%s",info);
      ground::send_text(cmdmsg);
      send_tlc(DET_EXPOSE_TLC, cmdmsg);
    };
    return;
  }; // command_exposure();

};
