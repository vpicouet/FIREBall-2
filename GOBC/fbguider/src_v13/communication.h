#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

// how long to sleep between polls (in microseconds)
#define POLL_SLEEP 10000

// Gondola computer definitions
#define GONDOLA_IP "172.20.4.2"
#define GONDOLA_PORT 3025

// this computer definitions 
#define GUIDER_IP "172.20.4.134"
// detector computer definitions
#define DETECTOR_IP "172.20.4.110"
#define DETECTOR_PORT 4025

// ground communication channel
#define GROUND_PORT_REMOTE "/dev/ttyS6"
#define GROUND_PORT_LOCAL  "/home/salo/FB/test0.tty" // ground testing w/o network
#define GROUND_BAUDRATE B9600

// stage communication channel
#define STAGE_PORT "/dev/tiptilt"// assigned symlink to serial adapter  "/dev/ttyUSB0"
//#define STAGE_PORT "/dev/ttyS0" // ground testing w/o network
#define STAGE_BAUDRATE B57600
#define STAGE_BLOCKING 0

// buffer sizes
#define UDPBUFSIZE 2048
#define SERIAL_BUFFER_SIZE 2048

// start and stop the communication channels
// the start communications function 
int communications_start(int local);
int communications_stop();

void *communications_thread_worker(void *communications_thread_data);

// functions to write to various devices
int write_to_gondola(char *buffer, int length);
int write_to_ground(char *buffer, int length);
int write_to_detector(char *buffer, int length);
int command_to_detector(char *buffer);
int info_to_detector(char *buffer);
int write_to_stage(char *buffer, int length);

// functions to read from various devices
int read_from_stage();
int read_buffer_from_stage(char *bf);

#endif // COMMUNICATION_H_
