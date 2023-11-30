
#include "parameters.h"
#include "fbcomm.h"
#include "fblog.h"

#include "mpfdata.h"
#include "detdata.h"
#include "gnddata.h"
#include "stagedata.h"


// 
typedef struct worker_thread_data_t{

} worker_thread_data_t;

namespace fbcomm{

  // which bits are being simulated? 
  char mpf_on=MPF_ON;
  char det_on=DET_ON;
  char gnd_on=GND_ON;
  char focus_on=FOCUS_STAGE_ON;
  char mask_on=MASK_STAGE_ON;

  // gondola variables from(f) and to(t)
  mpf_t f_mpf;
  mpf_t t_mpf;

  // gnd variables from(f) and to(t)
  gnd_t f_gnd;
  gnd_t t_gnd;

  // detector variables from(f) and to (t)
  det_t f_det;
  det_t t_det;

  // don't know how the stages are addressed, yet
  
  // worker thread data
  worker_thread_data_t wtd;
  
  // mutexes for working with various channels, just in case
  pthread_mutex_t mpf_mutex=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t det_mutex=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t gnd_mutex=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t stage_mutex=PTHREAD_MUTEX_INITIALIZER;
  
  // file descriptors for various communication channels
  int fd_udp, fd_mot, fd_gnd;

  // structures for sockets
  struct sockaddr_in myaddr_mpf;
  struct sockaddr_in myaddr_det;
  struct sockaddr_in addr_mpf;
  struct sockaddr_in addr_det;


  int commopen(){
    return 0;
  };

  int commclose(){
    return 0;
  };

  int MPF_CalculateCheckSum(unsigned char * ByteTab, int Size){
    return 0;
  };

  int GND_CalculateCheckSum(unsigned char * ByteTab, int Size){
    return 0;
  };

  int MPFWrite(char *buffer, int buflen){
    return 0;
  };

  int GNDWrite(char *buffer, int buflen){
    return 0;
  };

  int DETWrite(char *buffer, int buflen){
    return 0;
  };

  int MOTWrite(char *buffer, int buflen){
    return 0;
  };

  void *communications_worker_function(void *worker_function_data);


}; //namespace fbcomm
