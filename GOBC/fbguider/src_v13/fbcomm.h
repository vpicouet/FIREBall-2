#ifndef FBCOMM_H_
#define FBCOMM_H_

// few preliminary defines

// gondola communication
#define MPF_IP "172.20.4.2"
#define MPF_PORT 3025

// UDP Buffer
#define UDPBUFFSIZE 2048

// ground communication
#define TMTC_PORT "/dev/ttyS0"
#define TMTC_BAUDRATE B9600

// detector communication
#define DET_IP "127.0.0.1"
#define DET_PORT 3024
#define SERVICE_PORT 21234

// stage communication 
#define STAGE_PORT "/dev/ttyS1"
#define STAGE_BAUDRATE B9600

// timing loop delay in milliseconds
#define COMM_LOOP_DELAY 10


namespace fbcomm{
  // checksum computation for gondola and detector messages
  short MPF_CalculateCheckSum(unsigned char * ByteTab, int Size);
  
  // checksum computation for guider <--> ground messages
  char GND_CalculateCheckSum(unsigned char * ByteTab, int Size);
  
  // function to write a message to the gondola
  int MPFWrite(char *buffer, int buflen);

  // function to write a message to the detector
  int DETWrite(char *buffer, int buflen);
  
  // function to write a message to the ground
  int GNDWrite(char *buffer, int buflen);

  // function to write a command to the stages
  int MOTWrite(char *buffer, int buflen);

  // functions to open and close the communication channels
  int commopen();
  int commclose();

  // the worker function. Will get invoked from commopen.
  void *communications_worker_function(void *worker_function_data);

}; //namesoace fbcomm

#endif // FBCOMM_H_
