#ifndef GROUND_COMMANDS_H_
#define GROUND_COMMANDS_H_


#include "image_analysis.h"

#define GROUND_HEADER \
  unsigned char ssync;\
  unsigned short count;\
  unsigned short length;\
  unsigned char mid
  
#define GROUND_TAIL \
  unsigned char crc; \
  unsigned char esync

// message structures
#pragma pack(push)
#pragma pack(1)

typedef struct tc_set_short_t{
  GROUND_HEADER;
  int var_short;
  GROUND_TAIL;
} tc_set_int_t;
#define tc_set_fb_distol_t tc_set_short_t


// for setting exposure time 100
//typedef struct tc_set_exposure_time_t{
typedef struct tc_set_unsigned_int_t{
  GROUND_HEADER;
  unsigned int var_uint;
  GROUND_TAIL;
} tc_set_unsigned_int_t;

#define tc_set_exposure_time_t tc_set_unsigned_int_t


// for setting how we display
typedef struct tc_set_display_mode_t{
  GROUND_HEADER;
  unsigned char dispmode;
  GROUND_TAIL;
} tc_set_display_mode_t;

typedef struct tc_set_uchar_mode_t{
  GROUND_HEADER;
  unsigned char value;
  GROUND_TAIL;
} tc_set_uchar_mode_t;
#define tc_carousel_pos_t tc_set_uchar_mode_t
#define tc_star_box_cut_t tc_set_uchar_mode_t
#define tc_set_guider_mode_acquire_limited_t tc_set_uchar_mode_t
#define tc_set_guider_subtract_t tc_set_uchar_mode_t
#define tc_select_centroiding_algorithm_t tc_set_uchar_mode_t
//#define tc_set_focus_delay_t tc_set_uchar_mode_t
#define tc_refine_targets_t tc_set_uchar_mode_t
#define tc_compute_min_image_t tc_set_uchar_mode_t
#define tc_cpu_power_t tc_set_uchar_mode_t
#define tc_shutdown_guider_t tc_set_uchar_mode_t

// for focus
typedef struct tc_set_focus_delay_t{
  GROUND_HEADER;			     
  unsigned char delay;
  float step;
  GROUND_TAIL;
}tc_set_focus_delay_t;

// for selecting guider mode
typedef struct tc_set_guider_mode_t{
  GROUND_HEADER;
  unsigned char gmode;
  GROUND_TAIL;
} tc_set_guider_mode_t;

// target points
typedef struct tc_set_target_points_t{
  GROUND_HEADER;
  short x[MAXSTARS];
  short y[MAXSTARS];
  GROUND_TAIL;
} tc_set_target_points_t;


// for loading a target (105)
typedef struct tc_load_target_t{
  GROUND_HEADER;
  unsigned short targetid;
  GROUND_TAIL;
} tc_load_target_t;

// for adjusting the pointing slightly
typedef struct tc_adjust_pointing_t{
  GROUND_HEADER;
  float dx;
  float dy;
  float drot;
  GROUND_TAIL;
} tc_adjust_pointing_t;

typedef struct tc_adjust_single_star_t{
  GROUND_HEADER;
  unsigned char starid;
  float newx;
  float newy;
  GROUND_TAIL;
} tc_adjust_single_star_t;

// switch target and star pointing
typedef struct tc_switch_star_t{
  GROUND_HEADER;
  float x;
  float y;
  GROUND_TAIL;
} tc_switch_star_t;

// for changing which stars get used (54) 
// 120727 -- matmat
// for now assumes 8 stars max. 
typedef struct tc_change_used_stars_t{
  GROUND_HEADER;
  unsigned char usemask;
  GROUND_TAIL;
} tc_change_used_stars_t;


// change sigmas for objects (55)
// 120727 -- matmat
// encoding is unsigned short/2048.
typedef struct tc_change_guide_star_sigma_t{
  GROUND_HEADER;
  unsigned char thresh;
  unsigned short sigmin;
  unsigned short sigmax;
  GROUND_TAIL;
} tc_change_guide_star_sigma_t;

typedef struct tc_dither_t{
  GROUND_HEADER;
  unsigned char delay;
  unsigned char pattern;
  GROUND_TAIL;
}tc_dither_t;

// switch to next target or star (1=target, 2=star, 3=both)
typedef struct tc_switch_to_next_t{
  GROUND_HEADER;
  unsigned char switch_flag;
  GROUND_TAIL;
} tc_switch_to_next_t;

// convert next target to star
typedef struct tc_convert_next_target_to_star_t{
  GROUND_HEADER;
  float dx;
  float dy;
  float dr;
  GROUND_TAIL;
} tc_convert_next_target_to_star_t;

typedef struct tc_convert_next_target_to_star_sky_t{
  GROUND_HEADER;
  GROUND_TAIL;
} tc_convert_next_target_to_star_sky_t;


typedef struct tc_set_target_positions_t{
  GROUND_HEADER;
  short x[MAXSTARS];
  short y[MAXSTARS];
  GROUND_TAIL;
} tc_set_target_positions_t;

typedef struct tc_solve_field_t{
  GROUND_HEADER;
  unsigned char stack;
  unsigned short stacksize;
  unsigned char solve;
  GROUND_TAIL;
} tc_solve_field_t;

// for setting the stretch
typedef struct tc_set_display_stretch_t{
  GROUND_HEADER;
  int min;
  int max;
  float gamma;
  GROUND_TAIL;
} tc_set_display_stretch_t;

// for echo command 
typedef struct tm_echo_command_t{
  GROUND_HEADER;
  unsigned char id;
  GROUND_TAIL;
} tm_echo_command_t;


typedef struct tc_set_float_t{
  GROUND_HEADER;
  float var_float;
  GROUND_TAIL;
} tc_set_float_t;

#define tc_focus_delta_t tc_set_float_t
#define tc_mask_stage_position_t tc_set_float_t
#define tc_tiptilt_stage_position_t tc_set_float_t
#define tc_set_fb_thresh_t tc_set_float_t
#define tc_set_fb_gain_t tc_set_float_t
#define tc_set_fb_offset_t tc_set_float_t
#define tc_set_fb_rn_t tc_set_float_t

// stage commands 
/*
// these are commented out to use the tc_set_foat_t thing above
typedef struct tc_mask_stage_position_t{
  GROUND_HEADER;
  float absmove;
  GROUND_TAIL;
} tc_mask_stage_position_t;

typedef struct tc_tiptilt_stage_position_t{
  GROUND_HEADER;
  float absmove;
  GROUND_TAIL;
} tc_tiptilt_stage_position_t;

*/

typedef struct tm_stage_status_t{
  GROUND_HEADER;
  float stagerpos;
  float stage1pos;
  float stage2pos;
  float stage3pos;
  unsigned char stageerr[4];
  unsigned short poserr[4];
  unsigned char stagestate[4];
  GROUND_TAIL;
} tm_stage_status_t;


typedef struct tc_set_ushort_t{
  GROUND_HEADER;
  unsigned short var_ushort;
  GROUND_TAIL;
} tc_set_ushort_t;


#define tc_set_ttl_t tc_set_ushort_t
#define tc_set_fb_satval_t tc_set_ushort_t

#define tc_set_fb_gridsize_t tc_set_ushort_t
#define tc_set_fb_cenbox_t tc_set_ushort_t
#define tc_set_fb_apbox_t tc_set_ushort_t
#define tc_set_image_tag_t tc_set_ushort_t
#define tc_set_image_save_period_t tc_set_ushort_t

// for setting ttl (80/81)  ON/OFF
/*
typedef struct tc_set_ttl_t{
  GROUND_HEADER;
  unsigned short ttl_id;
  GROUND_TAIL;
} tc_set_ttl_t;
*/

typedef struct tm_gondola_boresight_t{
  GROUND_HEADER;
  float az,el,rot;
  GROUND_TAIL;
}tm_gondola_boresight_t;

typedef struct tm_dtu_radec_t{
  GROUND_HEADER;
  float ra,dec,roll;
  GROUND_TAIL;
}tm_dtu_radec_t;

typedef struct tm_guider_status_t{
  GROUND_HEADER;
  char guidemode;
  char dispmode;
  unsigned char algorithm;
  unsigned char subtract;
  unsigned long int frameno;
  unsigned short imgtag;
  int exptime;
  short ccdtemp;
  short camtemp;
  short pstemp;
  float temperature;
  float pressure;
  float gobc_temp;
  float gobc_pressure;
  float cal_temp;
  float cal_pressure;
  unsigned short saveperiod;
  GROUND_TAIL;
} tm_guider_status_t;

typedef struct tm_dio_status_t{
  GROUND_HEADER;
  unsigned char dio;
  GROUND_TAIL;
}tm_dio_status_t;

typedef struct star_profile_t{
  unsigned char starmask;
  long framenumber;
  //  unsigned short xprofile[(const int) IMG_BOXSIZE*2+1];
    unsigned short xprofile[IMG_BOXSIZE*2+1];
    unsigned short yprofile[IMG_BOXSIZE*2+1];
  // this is now oversized
  //  unsigned short xprofile[120];
  //unsigned short yprofile[120];
  float xfit[4];
  float yfit[4];
  float stagepos[4];
  unsigned short cx,cy;
  unsigned char cut;
}star_profile_t;

typedef struct tm_star_profiles_t{
  GROUND_HEADER;
  star_profile_t spt;
  GROUND_TAIL;
}tm_star_profiles_t;


// 170727 -- matmat
typedef struct tm_targets_t{
  GROUND_HEADER;
  unsigned short coords[MAXSTARS*2];
  unsigned char snrcut;
  unsigned short smin;
  unsigned short smax;
  GROUND_TAIL;
} tm_targets_t;

// 170727 -- matmat
typedef struct tm_stars_t{
  GROUND_HEADER;
  unsigned short coords[MAXSTARS*3];
  unsigned char sigmas[MAXSTARS*2];
  unsigned char use;
  unsigned char valid;
  GROUND_TAIL;
} tm_stars_t;

typedef struct tm_fb_info_t {
  GROUND_HEADER;
  unsigned short gain;
  uint16_t satval;
  unsigned short thresh;
  short distol;
  unsigned short gridsize;
  unsigned short cenbox;
  unsigned short apbox;
  short offset;
  unsigned short rn;
  GROUND_TAIL;
} tm_fb_info_t;

typedef struct tm_guide_stats_t {
  GROUND_HEADER;
  float dx;
  float dy;
  float dr;
  float rmsdx;
  float rmsdy;
  float rmsdr;
  float msx;
  float msy;
  unsigned char cnt;
  GROUND_TAIL;
} tm_guide_stats_t;


#pragma pack(pop)

// message defines


// implemented
#define TC_SET_EXPOSURE_TIME_ID 4
#define TC_SET_EXPOSURE_TIME_LENGTH 4

// change the display stretch 
#define TC_SET_DISPLAY_STRETCH_ID 20
#define TC_SET_DISPLAY_STRETCH_LENGTH 8

// subtract median or rows? 
#define TC_SET_GUIDER_SUBTRACT_ID 25
#define TC_SET_GUIDER_SUBTRACT_LENGTH 2



// change the display mode (either large or small) /
#define TC_SET_DISPLAY_MODE_ID 28
#define TC_SET_DISPLAY_MODE_LENGTH 1

#define TC_SET_GUIDER_MODE_ID 40
#define TC_SET_GUIDER_MODE_LENGTH 2

// convert next target to next star
#define TC_CONVERT_NEXT_TARGET_TO_STAR_ID 41
#define TC_CONVERT_NEXT_TARGET_TO_STAR_LENGTH 12

// use astrometry to convert next target to next star
#define TC_CONVERT_NEXT_TARGET_TO_STAR_SKY_ID 42
#define TC_CONVERT_NEXT_TARGET_TO_STAR_SKY_LENGTH 1


#define TC_SELECT_CENTROIDING_ALGORITHM_ID 43
#define TC_SELECT_CENTROIDING_ALGORITHM_LENGTH 2

// make a small change to the target points
#define TC_ADJUST_POINTING_ID 44
#define TC_ADJUST_POINTING_LENGTH 12

#define TC_SET_GUIDER_MODE_ACQUIRE_LIMITED_ID 45
#define TC_SET_GUIDER_MODE_ACQUIRE_LIMITED_LENGTH 2

// move a star to specified positions 
#define TC_ADJUST_SINGLE_STAR_ID 46
#define TC_ADJUST_SINGLE_STAR_LENGTH 9

// Computer reboot and power down
#define TC_CPU_POWER_ID 99
#define TC_CPU_POWER_LENGTH 2


// long message that contains 8 targets (loads it into the 
// next target structure
#define TC_SET_TARGET_POSITIONS_ID 47
#define TC_SET_TARGET_POSITIONS_LENGTH 32

// switch tracking to a different star in the field of view
#define TC_SWITCH_STAR_ID 48
#define TC_SWITCH_STAR_LENGTH 8

// load target file
#define TC_LOAD_TARGET_ID 49
#define TC_LOAD_TARGET_LENGTH 2

// switch to next target or next star, or both.
#define TC_SWITCH_TO_NEXT_ID 50
#define TC_SWITCH_TO_NEXT_LENGTH 1

// 170727 -- matmat
// select a different set of stars from the valid ones
#define TC_CHANGE_USED_STARS_ID 54
#define TC_CHANGE_USED_STARS_LENGTH 2

// 170727 -- matmat
// change spot width threshholds
// The encoding is in unsigned short val/2048.
#define TC_ADJUST_SIGMA_MINMAX_ID 55
#define TC_ADJUST_SIGMA_MINMAX_LENGTH 5

// turn LEDs on or OFF.
#define TC_SET_TTL_ON_ID 80
#define TC_SET_TTL_ON_LENGTH 2

#define TC_SET_TTL_OFF_ID 81
#define TC_SET_TTL_OFF_LENGTH 2

#define TC_SET_IMAGE_TAG_ID 90
#define TC_SET_IMAGE_TAG_LENGTH 3

#define TC_SET_IMAGE_SAVE_PERIOD_ID 91
#define TC_SET_IMAGE_SAVE_PERIOD_LENGTH 3

#define TC_REFINE_TARGETS_ID 92
#define TC_REFINE_TARGETS_LENGTH 2

#define TC_COLLECT_MIN_IMAGE_ID 93
#define TC_COLLECT_MIN_IMAGE_LENGTH 2

#define TC_DITHER_ID 94
#define TC_DITHER_LENGTH 3


// stack and solve
#define TC_SOLVE_FIELD_ID 117
#define TC_SOLVE_FIELD_LENGTH 6

// mask stage commands
#define TC_MASK_STAGE_HOME_ID 100
#define TC_MASK_STAGE_HOME_LENGTH 1

#define TC_MASK_STAGE_POSITION_ID 101
#define TC_MASK_STAGE_POSITION_LENGTH 5


#define TM_GONDOLA_BORESIGHT_ID 245
#define TM_GONDOLA_BORESIGHT_LENGTH 13
#define TM_GONDOLA_BORESIGHT_PERIOD 1000
#define TM_GONDOLA_BORESIGHT_OFFSET 123

#define TM_DTU_RA_DEC_ID 244
#define TM_DTU_RA_DEC_LENGTH 13
#define TM_DTU_RA_DEC_PERIOD 1000
#define TM_DTU_RA_DEC_OFFSET 223


#define TM_DIO_STATUS_ID 246
#define TM_DIO_STATUS_LENGTH 2

#define TM_GUIDER_STATUS_ID 247
#define TM_GUIDER_STATUS_LENGTH 54
#define TM_GUIDER_STATUS_PERIOD 1000
#define TM_GUIDER_STATUS_OFFSET 37

#define TM_TARGETS_ID 129
#define TM_GUIDER_TARGETS_PERIOD 1000
#define TM_GUIDER_TARGETS_OFFSET 50

#define TM_STARS_ID 128
#define TM_GUIDER_STARS_PERIOD 300
#define TM_GUIDER_STARS_OFFSET 22

#define TM_FB_INFO_ID 230
#define TM_FB_INFO_PERIOD 3000
#define TM_FB_INFO_OFFSET 10

#define TM_GUIDE_STATS_ID 240
#define TM_GUIDE_STATS_PERIOD 300
#define TM_GUIDE_STATS_OFFSET 77

#define TM_STAR_PROFILE_ID 249
#define TM_STAR_PROFILE_PERIOD 100
#define TM_STAR_PROFILE_OFFSET 60

// unimplemented
#define TC_STATUS_ID 0
#define TC_STATUS_LENGTH ZZ
#define TC_STATUS_PERIOD 10000
#define TC_STATUS_PERIOD_OFFSET 1

#define TC_VIDEO_STATUS_ID 1
#define TC_VIDEO_STATUS_LENGTH ZZ
#define TC_VIDEO_STATUS_PERIOD 10000
#define TC_VIDEO_STATUS_PERIOD_OFFSET 33

#define TC_TRACKING_STATUS_ID 2
#define TC_TRACKING_STATUS_LENGTH ZZ
#define TC_TRACKING_STATUS_PERIOD 1000
#define TC_TRACKING_STATUS_PERIOD_OFFSET 66

#define TC_GONDOLA_POSITION_ID 3
#define TC_GONDOLA_POSITION_LENGTH ZZ
#define TC_GONDOLA_POSITION_PERIOD 10000
#define TC_GONDOLA_POSITION_PERIOD_OFFSET 100

#define TC_STAR_BOX_CUT_ID 150
#define TC_STAR_BOX_CUT_LENGTH 2

#define TC_TIPTILT_STAGE_HOME_ID 110
#define TC_TIPTILT_STAGE_HOME_LENGTH 1

#define TC_TIPTILT_STAGE_2_HOME_ID 120
#define TC_TIPTILT_STAGE_2_HOME_LENGTH 1

#define TC_TIPTILT_STAGE_3_HOME_ID 130
#define TC_TIPTILT_STAGE_3_HOME_LENGTH 1

#define TC_TIPTILT_FOCUS_ID 132
#define TC_TIPTILT_FOCUS_LENGTH 2

#define TC_TIPTILT_ABCMOVES_ID 133
#define TC_TIPTILT_ABCMOVES_LENGTH 1

#define TC_CAROUSEL_POS_ID 135
#define TC_CAROUSAL_POS_LENGTH 2

#define TC_NOMINAL_FOCUS_ID 136
#define TC_NOMINAL_FOCUS_LENGTH 1

#define TC_FOCUS_DELTA_ID 137
#define TC_FOCUS_DELTA_LENGTH 5





#define TC_TIPTILT_STAGE_POSITION_ID 111
#define TC_TIPTILT_STAGE_POSITION_LENGTH 5 

#define TC_TIPTILT_STAGE_2_POSITION_ID 121
#define TC_TIPTILT_STAGE_2_POSITION_LENGTH 5 

#define TC_TIPTILT_STAGE_3_POSITION_ID 131
#define TC_TIPTILT_STAGE_3_POSITION_LENGTH 5 


#define TC_SET_FB_GAIN_ID 221
#define TC_SET_FB_GAIN_LENGTH 5

#define TC_SET_FB_SATVAL_ID 222
#define TC_SET_FB_SATVAL_LENGTH 5

#define TC_SET_FB_THRESH_ID 223
#define TC_SET_FB_THRESH_LENGTH 5

#define TC_SET_FB_DISTOL_ID 224
#define TC_SET_FB_DISTOL_LENGTH 5

#define TC_SET_FB_GRIDSIZE_ID 225
#define TC_SET_FB_GRIDSIZE_LENGTH 5

#define TC_SET_FB_CENBOX_ID 226
#define TC_SET_FB_CENBOX_LENGTH 5

#define TC_SET_FB_APBOX_ID 227
#define TC_SET_FB_APBOX_LENGTH 5

#define TC_SET_FB_OFFSET_ID 228
#define TC_SET_FB_OFFSET_LENGTH 5

#define TC_SET_FB_RN_ID 229
#define TC_SET_FB_RN_LENGTH 5

#define TC_RESET_FB_ID 230
#define TC_RESET_FB_LENGTH 1

#define TC_ROTATION_CENTER_ID 113
#define TC_ROTATION_CENTER_LENGTH ZZ

#define TC_PLATE_SCALE_ID 114
#define TC_PLATE_SCALE_LENGTH ZZ

#define TC_TRACKSTAR_NUMBER_ID 115
#define TC_TRACKSTAR_NUMBER_LENGTH ZZ

#define TC_STACK_SIZE_ID 116
#define TC_STACK_SIZE_LENGTH ZZ


#define TC_SHUTDOWN_GUIDER_ID 140
#define TC_SHUTDOWN_GUIDER_LENGTH 2

#define TC_FIELD_SOLUTION_ID 200
#define TC_FIELD_SOLUTION_LENGTH ZZ

#define TC_STAGE_STATUS_ID 248
#define TC_STAGE_STATUS_LENGTH 17

#define TC_COMMAND_ECHO_ID 254
#define TC_COMMAND_ECHO_LENGTH 1




#endif // GROUND_COMMANDS_H_
