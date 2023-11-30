
#include <cstdio>
#include <cstdlib>

#include <sys/unistd.h>
#include <sys/un.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <cstring>
#include <math.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#include "parameters.h"
#include "communication.h"
#include "motors.h"
#include "adio.h"

#include "fblog.h"

#include "gondola_communication.h"
#include "ground_communication.h"
#include "detector_communication.h"

//#include "gt_message_definitions.h"
//#include "gm_message_definitions.h"
//#include "gd_message_definitions.h"

// file descriptors for various ports:
// udp used for both the gondola and detector
int fd_mpf, fd_det;
// motors and ground
int fd_mot, fd_gnd;

// channel status bits
char mpf_open, mpf_used;
char det_open, det_used, det_command;
char gnd_open, gnd_used;
char mot_open, mot_used;

// buffers
char stage_buffer[SERIAL_BUFFER_SIZE];
char gnd_buffer[SERIAL_BUFFER_SIZE];
unsigned char mpf_buffer[UDPBUFSIZE];
unsigned char det_buffer[UDPBUFSIZE];

// structures for sockets
struct sockaddr_in myaddr_mpf;
struct sockaddr_in myaddr_det;
struct sockaddr_in myaddr_gnd;
struct sockaddr_in addr_mpf;
struct sockaddr_in addr_tmtc;
struct sockaddr_in addr_det;

// fd_set for polling
struct pollfd fds[3];

// worker threads
pthread_t communications_thread;
pthread_t motor_thread;
int communications_thread_run, motor_thread_run;

// mutexes for general happiness
pthread_mutex_t mpf_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t det_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gnd_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mot_mutex=PTHREAD_MUTEX_INITIALIZER;

int communications_start(int local){
  //  struct sockaddr_in remaddr; // remote address
  //  socklen_t addrlen = sizeof(remaddr);
  //  int i;
  char server[255];
  // configuration structures for TMTC
  struct termios oldtioTMTC, newtioTMTC;
  struct termios oldtioMOT, newtioMOT;

  // set the variables
  gnd_open = 0;
  det_open = 0;
  mpf_open = 0;
  mot_open = 0;
  // these are defined in parameters.h
  gnd_used = GND_ON;
  mpf_used = MPF_ON;
  det_used = DET_ON;
  mot_used = MASK_STAGE_ON | FOCUS_STAGE_ON;
  
  // are we allowed to command the detector from the start? NO!
  det_command = 0;

  // only open the mpf channel, if the mpf is turned on
  if (mpf_used){
    mpf_open=1;
    if ((fd_mpf = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      fblog::logerr("Cannot create MPF socket: %s",strerror(errno));
      mpf_open=0;
      mpf_used=0;
      // negative file descriptor makes poll() ignore this channel
      fds[0].fd = -1;
    } else { 
      /* bind the socket to any valid IP address and a specific port */
      memset((char *)&myaddr_mpf, 0, sizeof(myaddr_mpf));
      myaddr_mpf.sin_family = AF_INET;
      myaddr_mpf.sin_addr.s_addr = htonl(INADDR_ANY);
      myaddr_mpf.sin_port = htons(GONDOLA_PORT);
      fds[0].fd = fd_mpf;
      fds[0].events = POLLRDNORM;
      if (bind(fd_mpf, (struct sockaddr *)&myaddr_mpf, sizeof(myaddr_mpf)) < 0) {
	fblog::logerr("Cannot bind MPF socket: %s",strerror(errno));
	mpf_open = 0;
	mpf_used =0;
	fds[0].fd = -1;
      } else { 
	memset((char *) &addr_mpf, 0, sizeof(addr_mpf));
	addr_mpf.sin_family = AF_INET;
	addr_mpf.sin_port = htons(GONDOLA_PORT);
	sprintf(server,"%s",GONDOLA_IP);
	if (inet_pton(AF_INET, server, &addr_mpf.sin_addr)==0) {
	  fblog::logerr("Cannot configure MPF address: %s",strerror(errno));
	  mpf_open = 0;
	  mpf_used = 0;
	  fds[0].fd = -1;
	}; // inet_pton
      };  //bind
    }; // created socket OK.
  }; // mpf_used
  
  if (mpf_open){
    fblog::logmsg("MPF Communication initialized.");
  } else { 
    fblog::logmsg("MPF Communication NOT initialized.");
  };//mpf_open
  
  if (det_used) {
    det_open=1;

    if ((fd_det = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      det_used = 0;
      det_open = 0;
      fds[1].fd = -1;
      fblog::logerr("Cannot create DET socket: %s",strerror(errno));
    } else { 
    /* bind the socket to any valid IP address and a specific port */
      fds[1].fd = fd_det;
      fds[1].events = POLLRDNORM;
      memset((char *)&myaddr_det, 0, sizeof(myaddr_det));
      myaddr_det.sin_family = AF_INET;
      myaddr_det.sin_addr.s_addr = htonl(INADDR_ANY);
      myaddr_det.sin_port = htons(DETECTOR_PORT);
      
      if (bind(fd_det, (struct sockaddr *)&myaddr_det, sizeof(myaddr_det)) < 0) {
      fblog::logerr("Cannot bind DET socket: %s",strerror(errno));
      } else {
	memset((char *) &addr_det, 0, sizeof(addr_det));
	addr_det.sin_family = AF_INET;
	addr_det.sin_port = htons(DETECTOR_PORT);
	sprintf(server,"%s",DETECTOR_IP);
	if (inet_pton(AF_INET, server, &addr_det.sin_addr)==0) {
	  fblog::logerr("Cannot configure DET address: %s",strerror(errno));
	  det_open = 0;
	  det_used = 0;
	  fds[1].fd = -1;
	}; // inet_pton fail
      }; // bind successful
    }; // socket failed
  }; //det_used
  
  if (det_open){
    fblog::logmsg("DET Communication initialized.");
  } else { 
    fblog::logmsg("DET Communication NOT initialized.");
  };//det_open
  
  
  // only open the detector channel, if the detector is on
  
  if(gnd_used){
    gnd_open = 1;   
    if (local){
      fd_gnd = open(GROUND_PORT_LOCAL, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK | O_ASYNC);
    } else { 
      fd_gnd = open(GROUND_PORT_REMOTE, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK | O_ASYNC);
    };


    if(fd_gnd < 0){                  
      fblog::logerr("Could not open GND serial stream (%d).",fd_gnd);
      gnd_open = 0;
      gnd_used = 0;
      fds[2].fd = -1;
    } else {  // gnd connection failed to open
      fds[2].fd = fd_gnd;
      //fds[2].events = POLLRDNORM;
      fds[2].events = POLLIN | POLLRDNORM;
      // configure the port
      tcgetattr(fd_gnd, &oldtioTMTC);
      newtioTMTC.c_cflag = GROUND_BAUDRATE | CS8 | CLOCAL| CREAD;
      // onsky newtioTMTC.c_iflag = IGNPAR ;
      newtioTMTC.c_iflag = IGNPAR;
      newtioTMTC.c_iflag &= ~( ICRNL |
			  INLCR | PARMRK | INPCK | ISTRIP | IXON);
      // sky newtioTMTC.c_iflag = IGNPAR| ICRNL; 
      newtioTMTC.c_oflag = 0;
      newtioTMTC.c_lflag |= 0;
      // onground newtioTMTC.c_lflag = ICANON;
      newtioTMTC.c_cc[VMIN] = 0;
      newtioTMTC.c_cc[VTIME] = 0;
      newtioTMTC.c_cc[VTIME] = 0;
      tcflush(fd_gnd, TCIFLUSH);
      tcsetattr(fd_gnd, TCSANOW, &newtioTMTC);     
    }; // GND opened fine
  }; // gnd_used

  if (gnd_open){
    fblog::logmsg("GND Communication initialized.");
  } else { 
    fblog::logmsg("GND Communication NOT initialized.");
  };//gnd_open


  // motor port settings taken from
  // ~/SMC100/serial.[hc] 
  // by NdM
  if (mot_used){
    mot_open = 1;
    fd_mot = open(STAGE_PORT,O_RDWR | O_NOCTTY | O_NONBLOCK);
    //sky    fd_mot = open(STAGE_PORT,O_RDWR | O_NOCTTY);
    if(fd_mot < 0){
      mot_open = 0;
      mot_used = 0;
      fblog::logerr("Could not open MOT serial stream. (%d)",fd_mot);
    } else { // error opening port
      if (tcgetattr(fd_mot, &oldtioMOT) != 0){
	fblog::logerr("Could not configure MOT port");
      } else { // error configuring
	memset(&newtioMOT,0,sizeof(newtioMOT));
	tcflush(fd_mot, TCIOFLUSH);
	// set the baud rate
	cfsetispeed(&newtioMOT, STAGE_BAUDRATE);
	cfsetospeed(&newtioMOT, STAGE_BAUDRATE);
	// set flow control
	newtioMOT.c_iflag |= IXON;
	newtioMOT.c_iflag |= IXOFF;
	// Set up 8N1
	newtioMOT.c_cflag &= ~PARENB;
	newtioMOT.c_cflag &= ~CSTOPB;
	newtioMOT.c_cflag &= ~CSIZE;
	newtioMOT.c_cflag |= CS8;
	// turn on read
	newtioMOT.c_cflag |= CREAD;
	newtioMOT.c_cflag |= CLOCAL;
	// set blocking
	newtioMOT.c_cc[VMIN] = STAGE_BLOCKING;
	newtioMOT.c_cc[VTIME] = 5;
	// set termination character to CR NL
	newtioMOT.c_oflag |= OCRNL;
	if (tcsetattr(fd_mot, TCSANOW,&newtioMOT) !=0) {
	  fblog::logerr("Can't set MOT attributes.");
	  mot_used = 0;
	  mot_open = 0;
	} else {
	  tcflush(fd_mot, TCIOFLUSH);
	};
      }; // no error configuring
    }; // no error opening port
  }; //mot_used  


  communications_thread_run = 1;
  motor_thread_run =1;

  // finally, spin off the threads
  // communications thread
  pthread_create(&communications_thread, NULL,
		 communications_thread_worker, NULL);
  pthread_detach(communications_thread);

  // motor stage thread
  /*
    pthread_create(&motor_thread, NULL,
		 motors::motor_thread_worker, NULL);
  pthread_detach(motor_thread);
  */
  return 0;
}; // communications_start();

int communications_stop(){
  communications_thread_run = 0;
  motor_thread_run = 0;
  if (mot_open) close(fd_mot);
  if (gnd_open) close(fd_gnd);
  if (det_open) close(fd_det);
  if (mpf_open) close(fd_mpf);
  
  return 0;
}; // communications_stop()

void *communications_thread_worker(void *communications_thread_data){
  unsigned short counter;
  int extrasleep;
  //char message[255];
  struct sockaddr_in remaddr;
  socklen_t addrlen = sizeof(remaddr); // length of address;
  int recvlen;
  //  int idx;
  float pos;
  //  int runon=1;
  fblog::logmsg("Comm thread started.");
  counter = 0;
  while(communications_thread_run){
    // poll all the ports that are to be polled
    poll(fds,3,0);
    // check the MPF channel
    if(fds[0].revents & POLLRDNORM){
      //if(fds[0].revents & ( POLLRDNORM | POLLIN  )){
      recvlen = recvfrom(fd_mpf, mpf_buffer, 
			 UDPBUFSIZE, 0, 
			 (struct sockaddr*)&remaddr, &addrlen);
      //uncomment below
      gondola::process_gondola_messages(mpf_buffer, recvlen);
    }; // fds[0].revents
    if(fds[1].revents & POLLRDNORM) { 
      //if(fds[1].revents & ( POLLRDNORM | POLLIN )) { 
      recvlen = recvfrom(fd_det, det_buffer, UDPBUFSIZE, 0,
			 (struct sockaddr*)&remaddr, &addrlen);
      //uncomment below
      //det_process_message(&det_buffer, recvlen);
    };// fds[1].revents
    //    if (fds[2].revents & POLLRDNORM) { 
    if (fds[2].revents & (POLLIN | POLLRDNORM |POLLOUT )) { 
      recvlen = read(fd_gnd, gnd_buffer, SERIAL_BUFFER_SIZE);
      //printf("RCVD %d,  ",recvlen);
      // uncomment below
      if(recvlen>0){
	//	for(int i=0;i<recvlen;i++)
	//	  printf("%d ",gnd_buffer[i]);
	//	printf("\n");
	ground::push_ground_message(gnd_buffer, recvlen);
      }; //recvlen > 0
    }; //fds[2].revents
    // GROUND MESSAGE INPUT
    // look at one command at a time
    // this assumes the command rate is pretty low
    // need an if statement for ground message stuff
    if (counter % GROUND_PROCESS_PERIOD == GROUND_PROCESS_OFFSET)
      ground::process_ground_message();



    // GONDOLA MESSAGE OUTPUT
    if(counter % GG_STATUS_PERIOD == GG_STATUS_PERIOD_OFFSET){
      gondola::send_guider_status();
    }


    if(counter % GG_GUIDER_BORESIGHT_PERIOD == GG_GUIDER_BORESIGHT_PERIOD_OFFSET){
      gondola::send_guider_boresight();
      fblog::logmsg("a-d-r written");
    };
    
    if(counter % GG_GUIDER_ROTERROR_PERIOD == GG_GUIDER_ROTERROR_PERIOD_OFFSET){
      gondola::send_guider_rotation_error();
    };


    //    DEBUGPRINT("%d\n",counter);
    // loop over various messages to send
    
    // DET
    // GND
    
   
    if(counter % MOTOR_POLL == 10){
      // update position
      motors::getpos(ROTAXIS, &pos);
    };

    if(counter % MOTOR_POLL == 20){
      // get move error
      motors::geterr(ROTAXIS);
    };

    if(counter % MOTOR_POLL == 30){
      // get status 
      motors::getstatus(ROTAXIS);
    };

    if(counter % MOTOR_POLL == 40){
      motors::getpos(LINAAXIS, &pos);
    };

    if(counter % MOTOR_POLL == 50){
      // get move error
      motors::geterr(LINAAXIS);
    };

    if(counter % MOTOR_POLL == 60){
      // get status 
      motors::getstatus(LINAAXIS);
    };

    if(counter % MOTOR_POLL == 70){
      motors::getpos(LINBAXIS, &pos);
    };

    if(counter % MOTOR_POLL == 80){
      // get move error
      motors::geterr(LINBAXIS);
    };

    if(counter % MOTOR_POLL == 90){
      // get status 
      motors::getstatus(LINBAXIS);
    };

    if(counter % MOTOR_POLL == 100){
      motors::getpos(LINCAXIS, &pos);
    };

    if(counter % MOTOR_POLL == 110){
      // get move error
      motors::geterr(LINCAXIS);
    };

    if(counter % MOTOR_POLL == 120){
      // get status 
      motors::getstatus(LINCAXIS);
    };

    if(counter % MOTOR_POLL == 140){
      ground::send_stage_status();
      detector::send_stageinfo();
    };

    // write stage positions to the log.
    if(counter % MOTOR_POLL == 140){
      fblog::logmsg("POS %.3lf %.3lf %.3lf %.3lf",motors::get_motorpos(ROTAXIS),motors::get_motorpos(LINAAXIS),motors::get_motorpos(LINBAXIS),motors::get_motorpos(LINCAXIS));
    };

    if (counter % FOCUS_POLL == 77){
      motors::focus_step();
    };

    if( counter % DITHER_POLL == 25){
      motors::dither_step();
    };


    // loop over messages to send to the motors
    // loop over messages to receive from motors
    // MOT OUT
    // MOT IN

    // CARD IO sources
    if(counter % ADIO_POLL == 10){
      static unsigned char in;
      //      printf("Reading digital in!\n");
      adio::digital_in(&in);
    };
    if(counter % ADIO_POLL == 20){
      adio::update_analog_in();
    };
    // card IO stuff
    // guider status
    if(counter % TM_GUIDER_STATUS_PERIOD == TM_GUIDER_STATUS_OFFSET){
      ground::send_guider_status();
      DEBUGPRINT("Sending guider status\n");
    }; // guider status

    if(counter % TM_GUIDER_STATUS_PERIOD == TM_GUIDER_STATUS_OFFSET+15){
      ground::send_dio_status();
      detector::send_lampinfo();
      DEBUGPRINT("Sending DIO status\n");
    }; // guider status

    if(counter % TM_GUIDER_STATUS_PERIOD == TM_GUIDER_STATUS_OFFSET+25){
      detector::send_gondolainfo();
      DEBUGPRINT("Sending DIO status\n");
    }; // guider status

    if(counter % TM_GUIDER_STATUS_PERIOD == TM_GUIDER_STATUS_OFFSET+25){
      detector::send_tag();
      DEBUGPRINT("Sending DIO status\n");
    }; // guider status

    // send targets
    if (counter % TM_GUIDER_TARGETS_PERIOD == TM_GUIDER_TARGETS_OFFSET){
      ground::send_target_info();
      DEBUGPRINT("Sending target info\n");
    }; // send targets

    // send stars 
    if (counter % TM_GUIDER_STARS_PERIOD == TM_GUIDER_STARS_OFFSET){
      ground::send_star_info();
      DEBUGPRINT("Sending star info\n");
    }; // send stars
    
    // send frameblob info
    if ( counter % TM_FB_INFO_PERIOD == TM_FB_INFO_OFFSET) {
      ground::send_fb_info();
      DEBUGPRINT("Sending FB info\n");
    }; // send frameblob.info

    if ( counter % TM_GUIDE_STATS_PERIOD == TM_GUIDE_STATS_OFFSET) { 
      ground::send_guide_stats();
      DEBUGPRINT("Sending GuideStats\n");
    }; // send_guide_stats

    if ( counter % TM_GONDOLA_BORESIGHT_PERIOD == TM_GONDOLA_BORESIGHT_OFFSET ){
      ground::send_gondola_boresight();
    };// send_gondola_boresight();

    if ( counter % TM_DTU_RA_DEC_PERIOD == TM_DTU_RA_DEC_OFFSET ) {
      ground::send_dtu_radec();
    };;// send dtu ra/dec/roll

    if ( counter % TM_STAR_PROFILE_PERIOD == TM_STAR_PROFILE_OFFSET ){
      ground::send_star_profiles();
    };// 

    // take a nap
    counter++;
    usleep(POLL_SLEEP-extrasleep);
    extrasleep=0;
  };
  fblog::logmsg("Comm thread stopped.");
  return NULL;
};


int write_to_gondola(char *buffer, int length){
  static unsigned int slen;
  static unsigned int len = 0;
  if(mpf_open && mpf_used){
    slen=sizeof(addr_mpf);
    pthread_mutex_lock(&mpf_mutex);
    sendto(fd_mpf,buffer,length,0,(struct sockaddr *)&addr_mpf,slen);
    DEBUGPRINT("WRITE_TO_GONDOLA %d %d",buffer[0],length);
    pthread_mutex_unlock(&mpf_mutex);
    len=length;
  }; // mpf_open
  return len;
}; // write_to_gondola();

int write_to_ground(char *buffer, int length){
  static int len;
  len = 0;
  DEBUGPRINT("Attempting to write to ground.\n");
  if (gnd_open && gnd_used){
    pthread_mutex_lock(&gnd_mutex);
    len = write(fd_gnd, buffer, length);
    fdatasync(fd_gnd);
    DEBUGPRINT("Written to ground.\n");
    DEBUGPRINT("%d\n", len);
    pthread_mutex_unlock(&gnd_mutex);
  }; // gnd_open and used
  return len;
}; // write_to_ground();
		   
int write_to_detector(char *buffer, int length){
  static unsigned int len,slen;
  len = 0;
  if (det_open && det_used){
    DEBUGPRINT("%d\n",len);
    slen=sizeof(addr_det);
    pthread_mutex_lock(&det_mutex);
    sendto(fd_det,buffer,length,0,(struct sockaddr *)&addr_det,slen);
    pthread_mutex_unlock(&det_mutex);
  }; // det_open && used
  return len;
};// write_to_detector();

int write_to_stage(char *buffer, int length){
  if(mot_open){
    return write(fd_mot, buffer, length);
  } else {
    return 0;
  };
}; // write_to_stage();

int read_from_stage(){
  static int len;
  len = 0;
  memset(stage_buffer,0,SERIAL_BUFFER_SIZE);
  len = read(fd_mot, stage_buffer, SERIAL_BUFFER_SIZE);
  if(len>0 && len<SERIAL_BUFFER_SIZE-1) stage_buffer[len]=0;
  return len;
}; //read_from_stage

int read_buffer_from_stage(char *buf){
  static int len;
  if (mot_open){
    len  = read_from_stage();
    memcpy(buf,stage_buffer,len+1);
    return len;
  } else {
    return 0;
  };
}; // read from stage(char *buf)
