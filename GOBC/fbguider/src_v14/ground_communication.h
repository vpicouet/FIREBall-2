#ifndef GROUND_COMMUNICATION_H_
#define GROUND_COMMUNICATION_H_

#include <cstdint>

#include "communication.h"
#include "ground_commands.h"
#include "ringbuffer.h"
#include "image_analysis.h"

// macro for sending to ground
#define WRITE_TO_GROUND(msg) write_to_ground((char*) &msg, sizeof(msg))
#define COMPUTE_GROUND_CRC(msg) FindCRC((unsigned char *) &msg, sizeof(msg)-1)
#define GROUND_COUNTER() outbound_ground_counter++

// this is wrong:
#define MESSAGE_LENGTH(msg) sizeof(msg)


#define MIN_GROUND_MESSAGE_SIZE 8
#define MAX_GROUND_MESSAGE_SIZE 255
#define GROUND_PROCESS_PERIOD 10
#define GROUND_PROCESS_OFFSET 3



namespace ground{ 

  unsigned char FindCRC(char *msg, int size);
  unsigned char FindCRC(RingBuffer *rb, int size);
  
  int process_ground_message();
  int interpret_ground_message(unsigned char *msg, int length);
  int push_ground_message(char *msg, int size);


  void send_echo(unsigned char mid);
  void send_text(char *str);
  void send_video_status();
  void send_status();
  void send_stage_status();
  void send_gondola_boresight();
  void send_dtu_radec();
  void send_dio_status();
  void send_guider_status();
  

//170727 -- matmat
  void send_target_info();
  void send_star_info();
  void send_fb_info();
  void send_guide_stats();
 
  void send_star_profiles();

  
}; // namespace ground

// ground to guider



// guider to ground
void gt_status(int status);
void gt_displaystatus(int displaystatus);
void gt_commandreceived(unsigned char command);





#endif // GG_COMMUNICATION_H_
