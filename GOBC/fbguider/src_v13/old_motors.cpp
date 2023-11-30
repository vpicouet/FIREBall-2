#include <cstdio>
#include <cstdlib>

#include "motors.h"
#include "parameters.h"
#include "fblog.h"




namespace motors{

  int run_motor_thread;
  int axis;
  // stage structures
  

  // want to be careful with what is and is not allowed to move. 
  pthread_mutex_locker_t motor_motion_lock=PTHREAD_MUTEX_INITIALIZER;

  int configure_motor(int axis, motor_configuration_t *mct){

    return 0;
  };
  
  // toggle the motor. 
  int toggle_motor(int axis, int state){

    return 0;
  };

  int move_tip_tilt(int *positions){

    return 0;
  };

  int terminate_thread_worker(){
    run_motor_thread=0;
    return 0;
  };

  
  // worker thread function 
  // 
  void * motor_thread_worker(void *mtw_data){
    fblog::logmsg("Entering motor thread worker.");
    run_motor_thread=1;
    motor_command = Noop;
    
    while(run_motor_thread){
      // the main communications thread populates the command structures
      // 
      switch(motor_command){
      Home:
	break;
      Position:
	break;
      RelativePosition:
	break;
      GetPosition:
	break;
      Velocity:
	break;
      GetVelocity:
	break;
      Acceleration:
	break;
      GetAcceleration:
	break;
      GetStatus:
	break;
      Noop:
      Default:
	// Noop is the default.
      };
      
      
      


      usleep(MOTOR_THREAD_DELAY);
    };


    fblog::logmsg("Exiting motor thread worker.");
    return NULL;
  }; // motor_thread_worker
  
  // passing commands to the worker thread. 
  int home_axis(int axis_mask){
    if(motor_command == Noop){
      home_axis_struc.axis_mask = axis_mask;
      motor_command = Home;
      return 0;
    } else {
      return -1;
    };
    return 0;
  };
  
  void position_axis(int axis_mask,int position){
    if(motor_command == Noop){
      position_struc.axis_mask = axis_mask;
      position_struc.position = position;
      motor_command = Position;
      return 0;
    } else {
      return -1;
    };
    return 0;
  }; // position_axis

  void coordinated_position(int *position){
    if(motor_command == Noop){
      memcpy(coordinated_position_struc.position,position,(MOTOR_COUNT-1)*sizeof(int));
      motor_command = CoordinatedPosition;
      return 0;
    } else {
      return -1;
    };
  };

}; // namespace motors
