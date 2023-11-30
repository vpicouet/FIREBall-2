
#include "timing.h"
#include <sys/time.h>
#include <cstdlib>
#include <time.h>

double fb_start_time;
unsigned char time_started=0;

void start_time(){
  timeval tv;
  if(!time_started){
    gettimeofday(&tv,NULL);
    fb_start_time = tv.tv_sec + tv.tv_usec/1000000.;
    time_started=1;
  };
};


double get_time(){
  static timeval tv;
  static double timenow;
  if(!time_started) start_time();
  gettimeofday(&tv,NULL);
  timenow = tv.tv_sec + tv.tv_usec/1000000.;
  return timenow-fb_start_time;
}; // get_time

void message_time(int *sec, int *nsec){
  static timespec ts;
  clock_gettime(CLOCK_REALTIME,&ts);
  *sec = ts.tv_sec;
  *nsec = ts.tv_nsec;
  return;
}; // message_time

