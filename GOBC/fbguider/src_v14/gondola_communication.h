#ifndef GONDOLA_COMMUNICATION_H_
#define GONDOLA_COMMUNICATION_H_

#include <cstdint>

#include "communication.h"


// macro for sending to gondola
#define WRITE_TO_GONDOLA(msg) write_to_gondola((char*) &msg, sizeof(msg))
#define COMPUTE_GONDOLA_CRC(msg) compute_gondola_crc((unsigned char*) &msg, sizeof(msg)-2)
#define GONDOLA_COUNTER() outbound_counter++


#define MIN_MESSAGE_SIZE 8

// message IDs, length, and update period (in 10s of ms)
// negative frequency means it will not be updated
// the offset is there so not all of these messages get sent 
// during the same loop cycle

// status (note that this message can be sent in either direction).
#define GG_STATUS_ID 0x00
#define GG_STATUS_LENGTH 1
#define GG_STATUS_PERIOD 50
#define GG_STATUS_PERIOD_OFFSET 1

#define GG_DTU_DATA_ID 0x01
#define GG_DTU_DATA_LENGTH 50
#define GG_DTU_DATA_PERIOD -1000
#define GG_DTU_DATA_PERIOD_OFFSET 2

#define GG_GONDOLA_BORESIGHT_ID 0x02
#define GG_GONDOLA_BORESIGHT_LENGTH 21
#define GG_GONDOLA_BORESIGHT_PERIOD -1000
#define GG_GONDOLA_BORESIGHT_PERIOD_OFFSET 3

#define GG_GUIDER_BORESIGHT_ID 0x03
#define GG_GUIDER_BORESIGHT_LENGTH 21
#define GG_GUIDER_BORESIGHT_PERIOD 2000
#define GG_GUIDER_BORESIGHT_PERIOD_OFFSET 4

#define GG_GUIDER_ROTERROR_ID 0x04
#define GG_GUIDER_ROTERROR_LENGTH 13
#define GG_GUIDER_ROTERROR_PERIOD 3
#define GG_GUIDER_ROTERROR_PERIOD_OFFSET 5


// Message header and footer
#define GONDOLA_HEADER					\
  unsigned short counter;				\
  unsigned short length;				\
  unsigned char id

#define GONDOLA_FOOTER unsigned short crc

// message structure definitions
#pragma pack(push, 1)

// status message
// message 0x00
// sent in both directions
typedef struct gondola_status_t{
  GONDOLA_HEADER;
  unsigned char status;
  GONDOLA_FOOTER;
} gondola_status_t;

// message 0x01
// from gondola to guider
typedef struct gondola_DTU_data_t{
  GONDOLA_HEADER;
  unsigned char data[50];
  GONDOLA_FOOTER;
} gondola_DTU_data_t;

// message 0x02
//from gondola to guider
typedef struct gondola_ground_boresight_t{
  GONDOLA_HEADER;
  int32_t sec;
  int32_t nsec;
  float azimuth;
  float elevation;
  float rotation;
  unsigned char valid;
  GONDOLA_FOOTER;
} gondola_ground_boresight_t;

// message 0x03
// from guider to gondola
typedef struct guider_boresight_t{
  GONDOLA_HEADER;
  int32_t sec;
  int32_t nsec;
  float alpha;
  float delta;
  float rotation;
  unsigned char valid;
  GONDOLA_FOOTER;
} guider_boresight_t;

// message 0x04 
// from guider to gondola
typedef struct guider_rotation_error_t{
  GONDOLA_HEADER;
  int32_t sec;
  int32_t nsec;
  float roterror;
  unsigned char valid;
  GONDOLA_FOOTER;
} guider_rotation_error_t;



#pragma pack(pop)






namespace gondola{
  // return the timestamp
  inline void gondola_time(int32_t *sec, int32_t *nsec);
  
  // check if the counter value is consistent. if not, set
  unsigned char gondola_counter_received(unsigned short ctr);

  // status message handlers 0x00
  void update_guider_status(unsigned char status);
  void get_gondola_status(unsigned char *status);
  void send_guider_status();

  // DTU message 0x01
  // not processsed yet

  // gondola ground boresight 0x02
  char get_gondola_boresight(float *az, float *el, float *rot);
 
  // guider boresight 0x03
  void update_guider_boresight(float alpha, float delta, \
			       float rotation, unsigned char valid);
  void send_guider_boresight();

  // guider rotation error  0x04
  void update_guider_rotation_error(float roterror, unsigned char valid);
  void send_guider_rotation_error();
  
  // gondola CRC, stolen shamelessly from CSBF
  short compute_gondola_crc(unsigned char *ByteTab, int Size);
  
  // process a message buffer
  void process_gondola_messages(unsigned char *buf, int size);

  void extract_quaternions(unsigned char *data);

  void set_gondola_radec(double ira, double idec, double iroll);
  void get_gondola_radec(double *ora, double *odec, double *oroll);

}; // namespace_gondola;


#endif //GONDOLA_COMMUNICATION_H_
