#ifndef MOTORS_H_
#define MOTORS_H_
/* definitions and declarations for the fireball 
   mask, focus/tip-tilt stages */

// how many stages are there? 
#define MOTOR_COUNT 4

#define MOTOR_SLEEP 4000

// axes definitions
// note, there should be as many as there are in motor_count
typedef enum axis_t{
  mask = 1,
  linear0 = 2,
  linear1 = 4,
  linear2 = 8
} axis_t;


// structure to contain motor information
typedef struct motor_t{
  char controller_id;
  axis_t axis;
  unsigned char homed;
  int position;
  int nominal_position;
  int velocity, acceleration, deceleration;
  int state;
  float offset;
} motor_t;


// possible motor commands
typedef enum motor_command_t{
  Noop=0,
  Home=1,
  Position=2,
  CoordinatedPosition=3,
  RelativePosition=4,
  GetPosition=5,
  Velocity=6,
  GetVelocity=7,
  Acceleration=8,
  GetAcceleration=9,
  GetStatus=10
} motor_command_t;




// how frequently do we want to poll the stage positions (us)
#define MOTOR_POLL 50000000
#define MOTOR_THREAD_DELAY 500000

// how long to wait for a response? in us.
#define MOTOR_RESPONSE 150000



#define MASKID 0
#define FOCUS0ID 1
#define FOCUS1ID 2
#define FOCUS2ID 3

#define HOME_STRING "or\n"
#define POLL_STRING "tp\n"

#define MASK_HOME_STRING MASKID HOME_STRING
#define FOCUS0_HOME_STRIN FOCUS0ID HOME_STRING
#define FOCUS1_HOME_STRING FOCUS1ID HOME_STRING
#define FOCUS2_HOME_STRING FOCUS2ID HOME_STRING

#define MASK_POLL_STRING 
#define FOCUS0_POLL_STRING


namespace motors{

  void home_stage(int axis);
  void home_linear_stages();

  void set_stage_offset(int axis, float offset);
  void get_stage_offset(int axis, float *offset);

  void set_position(int axis, int position);
  void get_position(int axis, int *position);
  void get_status(int axis, int *status);

  void move_linear_stages(int *stages);
  
  


void set_mask_offset(float offset);


 void *motor_thread_worker(void *mtw_data);

}; //namespace motors


#endif // MOTORS_H_
