
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "server.h" // socket server
#include "camera.h" // camera processing commands

int ROS;
int volatile runon=1;

void intHandler(int){
  printf("\n\nCTRL-C Caught.\n\n");
  runon = 0;
  close_server();
  return;
};


void parseCommandLine(int argc, char **argv);
int main(int argc, char *argv[])
{
//int main(){
  int err;
  int runloop=1;
  int ROS;

  if (argc == 2) {
    ROS = atoi(argv[1]);
  } else { 
    ROS=-1;
  };
  signal(SIGINT, intHandler);
  
  if((err=open_server())){
    printf("Could not open server\n");
    return -1;
  }; // open server

  if((err=open_camera(ROS))){
    printf("Could not open camera\n");
    return -1;
  }; // open camera
  
  // listen server returns an integer.
  while(runloop && runon){
    runloop = listen_server();
  }; // runloop;

  close_camera();
  close_server();
  
  printf("Done.\n");
}; // main
