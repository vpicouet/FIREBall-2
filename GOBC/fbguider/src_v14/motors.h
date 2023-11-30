#ifndef MOTORS_H_
#define MOTORS_H_

#include "parameters.h"

#define MOTOR_SLEEP 50000
#define MOTOR_POLL 150

#define FOCUS_POLL 100
#define DITHER_POLL 50
#define ABC_POLL 100

#define NEWPORT_TELL_ERROR "TE"
#define NEWPORT_TELL_STATUS "TS"
#define NEWPORT_TELL_POSITION "TP"
#define NEWPORT_REL_MOVE "PR"
#define NEWPORT_ABS_MOVE "PA"
#define NEWPORT_HOME "OR"

// Adding hysteresis compensation preset -- ICA 06/23/23
//#define NEWPORT_BH "BH"
//#define NEWPORT_BH_PRESET 1

#define LIN_LIMIT_MIN 0.0
#define LIN_LIMIT_MAX 25.0

#define ROT_LIMIT_MIN -179.5
#define ROT_LIMIT_MAX 179.5


typedef enum axis_t{
  ROTAXIS = 1,
  LINAAXIS = 2,
  LINBAXIS = 3,
  LINCAXIS = 4
} axis_t;


typedef struct focus_structure_t{
  int do_focus;
  int counter;
  float xi[3];
  float xf[3][12];
  double delay;
  double starttime;
  float step;
  unsigned short saveperiod;
  unsigned char tookpicture;
} focus_structure_t;

typedef struct dither_structure_t{
  int do_dither;
  unsigned char pattern;
  int counter;
  unsigned char points;
  float txi[MAXSTARS], tyi[MAXSTARS];
  float txd[255][MAXSTARS], tyd[255][MAXSTARS];
  float dx[255],dy[255];
  double delay;
  double timetag;
  unsigned short saveperiod;
  unsigned char tookpicture;
} dither_structure_t;

typedef struct abc_moves_structure_t{
  int do_abc_moves;
  unsigned char pattern;
  int counter;
  unsigned char points;
  float posAi, posBi, posCi;
  float tAd[255], tBd[255], tCd[255];
  float posA[255],posB[255],posC[255];
  float delay;
  double starttime;
  unsigned short saveperiod;
  unsigned char tookpicture;
} abc_moves_structure_t;

typedef struct motor_positions_t {
  float stagepos[4];
  unsigned char stageerr[4];
  unsigned char stagestat[4];
  unsigned short poserr[4];
} motor_positions_t;

namespace motors{ 

  char home(axis_t axis);
  char absmove(axis_t axis, float position);
  char relmove(axis_t axis, float position);
  char getpos(axis_t axis, float *position);


  char set_carousel_station(unsigned char station);
  char nominal();

  char focus();

  void start_focus(char delay, float step);
  void start_abc();
  void focus_step();
  void end_focus();
  unsigned char is_focus();

			   

  void start_dither(char delay,unsigned char pattern);
  void dither_step();
  void end_dither();
  unsigned char is_dither();
  char load_dither_pattern(unsigned char pattern);
			   //  char load_dither_pattern(unsigned char pattern, 
			   // float *startx, float *starty,
			   //float *arrtx, float *arrty,
			   //unsigned char *pts);
			   


  char geterr(axis_t axis);
  char getstatus(axis_t axis);

  float get_motorpos(axis_t axis);
  void get_motor_positions(float *pos);
  // set motorpos should not be used outside of motors.cpp
  void set_motorpos(axis_t axis, float pos);

  void set_stage_error(axis_t axis,unsigned char err);
  void set_stage_status(axis_t axis, unsigned short poserr, unsigned char stagestat);
  void get_stage_statuses(unsigned char *stagerr, unsigned short *poserr, unsigned char *stagestat);

  void set_focus_delay(unsigned char delay);
  char focus_delta(float dx);

  char load_abc_moves();
  void start_abc();
  void abc_moves_step();
  void end_abc_moves();
  unsigned char is_abc_moves();

}; // namespace motors



#endif // MOTORS_H_
