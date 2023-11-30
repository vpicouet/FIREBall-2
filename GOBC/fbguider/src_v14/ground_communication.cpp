#include <cstdio>
#include <cstdlib>

#include "ground_communication.h"
#include "ground_commands.h"
#include "gondola_communication.h"
#include "communication.h"
#include "ringbuffer.h"

#include "fbvideo.h"
#include "fblog.h"
#include "fbcamera.h"

#include "image_analysis.h"

#include "motors.h"
#include "adio.h"

#define MAXBUFFERCOUNTER 100

namespace ground{

  // buffer to contain the messages
  RingBuffer gbuffer;

  // counters
  unsigned short outbound_ground_counter, inbound_ground_counter;
  
  // variables and structures for message processing
  tc_set_exposure_time_t tc_set_exposure_time;
  tc_set_display_mode_t tc_set_display_mode;
  tc_set_display_stretch_t tc_set_display_stretch;
  tc_set_guider_mode_t tc_set_guider_mode;
  tc_set_guider_mode_acquire_limited_t tc_set_guider_mode_acquire_limited;
  tc_select_centroiding_algorithm_t tc_select_centroiding_algorithm;
  tc_load_target_t tc_load_target;
  tc_set_target_points_t tc_set_target_points;
  tc_set_target_positions_t tc_set_target_positions;
  tc_change_used_stars_t tc_change_used_stars; // 120727 -- matmat
  tc_change_guide_star_sigma_t tc_change_guide_star_sigma; // 120727 -- matmat
  tc_adjust_pointing_t tc_adjust_pointing;
  tc_adjust_single_star_t tc_adjust_single_star; // tell the system where to move a particular star. 
  tc_switch_star_t tc_switch_star;
  tc_switch_to_next_t tc_switch_to_next;
  tc_solve_field_t tc_solve_field;
  tc_set_ttl_t tc_set_ttl;
  tc_convert_next_target_to_star_t tc_convert_next_target_to_star;
  tc_convert_next_target_to_star_sky_t tc_convert_next_target_to_star_sky;
  tc_carousel_pos_t tc_carousel_pos;
  tc_star_box_cut_t tc_star_box_cut;
  tc_set_image_tag_t tc_set_image_tag;
  tc_set_image_save_period_t tc_set_image_save_period;
  tc_set_focus_delay_t tc_set_focus_delay;
  tc_focus_delta_t tc_focus_delta;
  tc_compute_min_image_t tc_compute_min_image;
  tc_refine_targets_t tc_refine_targets;
  tc_mask_stage_position_t tc_mask_stage_position;
  tc_tiptilt_stage_position_t tc_tiptilt_stage_position;
  tc_dither_t tc_dither;
  tc_shutdown_guider_t tc_shutdown_guider;

  unsigned char ground_message_length[] = { 
    0, /* command   0 */	
    0, /* command   1 */
    0, /* command   2 */
    0, /* command   3 */
    5, /* command   4 : TC_SET_EXPOSURE_TIME ci [OK] */
    0, /* command   5 */
    0, /* command   6 */
    0, /* command   7 */
    0, /* command   8 */
    0, /* command   9 */
    0, /* command  10 */
    0, /* command  11 */
    0, /* command  12 */
    0, /* command  13 */
    0, /* command  14 */
    0, /* command  15 */
    0, /* command  16 */
    0, /* command  17 */
    0, /* command  18 */
    0, /* command  19 */
    13, /* command  20 : TC_SET_DISPLAY_STRETCH [OK] */
    0, /* command  21 */
    0, /* command  22 */
    0, /* command  23 */
    0, /* command  24 */
    2, /* command  25 : TC_SET_GUIDER_SUBTRACT [OK] */
    0, /* command  26 */
    0, /* command  27 */
    2, /* command  28 : TC_SET_DISPLAY_MODE [OK] */
    0, /* command  29 */
    0, /* command  30 */
    0, /* command  31 */
    0, /* command  32 */
    0, /* command  33 */
    0, /* command  34 */
    0, /* command  35 */
    0, /* command  36 */
    0, /* command  37 */
    0, /* command  38 */
    0, /* command  39 */
    2, /* command  40 : TC_SET_GUIDER_MODE [OK] */
    13, /* command  41 : TC_CONVERT_NEXT_TARGET_TO_NEXT_STAR [OK] */
    1, /* command  42 : TC_CONVERT_NEXT_TARGET_TO_STAR_SKY [OK] */
    2, /* command  43 : TC_SELECT_CENTROIDING_ALGORITHM [OK] */
    13, /* command  44 : TC_ADJUST_POINTING [OK] */
    2, /* command  45 : TC_SET_GUIDER_MODE_ACQUIRE_LIMITED [OK] */
    10, /* command  46 : TC_ADJUST_SINGLE_STAR [OK] */
    33, /* command  47 : TC_SET_TARGET_POSITIONS [OK] */
    9, /* command  48 : TC_SWITCH_STAR [OK] */
    3, /* command  49 : TC_LOAD_TARGET [OK] */
    2, /* command  50 : TC_SWITCH_TO_NEXT [OK] */
    0, /* command  51 */
    0, /* command  52 */
    0, /* command  53 */
    2, /* command  54 : TC_CHANGE_USED_STARS [OK] */
    6, /* command  55 : TC_ADJUST_SIGMA_MINMAX [OK] */
    0, /* command  56 */
    0, /* command  57 */
    0, /* command  58 */
    0, /* command  59 */
    0, /* command  60 */
    0, /* command  61 */
    0, /* command  62 */
    0, /* command  63 */
    0, /* command  64 */
    0, /* command  65 */
    0, /* command  66 */
    0, /* command  67 */
    0, /* command  68 */
    0, /* command  69 */
    0, /* command  70 */
    0, /* command  71 */
    0, /* command  72 */
    0, /* command  73 */
    0, /* command  74 */
    0, /* command  75 */
    0, /* command  76 */
    0, /* command  77 */
    0, /* command  78 */
    0, /* command  79 */
    2, /* command  80 : TC_SET_TTL_ON [OK]*/
    2, /* command  81 : TC_SET_TTL__OFF* [OK] */
    0, /* command  82 */
    0, /* command  83 */
    0, /* command  84 */
    0, /* command  85 */
    0, /* command  86 */
    0, /* command  87 */
    0, /* command  88 */
    0, /* command  89 */
    3, /* command  90 : TC_SET_IMAGE_TAG [OK] */
    3, /* command  91 : TC_SET_IMAGE_SAVE_PERIOD [OK] */
    2, /* command  92 : TC_REFINE_TARGETS [OK] */
    2, /* command  93 : TC_COLLECT_MIN_IMAGE [OK] */
    3, /* command  94 : TC_DITHER [OK] */
    0, /* command  95 */
    0, /* command  96 */
    0, /* command  97 */
    0, /* command  98 */
    2, /* command  99 : TC_CPU_POWER_ID */
    1, /* command 100 : TC_MASK_STAGE_HOME [OK] */
    5, /* command 101 : TC_MASK_STAGE_POSITION [OK]*/
    0, /* command 102 */
    0, /* command 103 */
    0, /* command 104 */
    0, /* command 105 */
    0, /* command 106 */
    0, /* command 107 */
    0, /* command 108 */
    0, /* command 109 */
    1, /* command 110 : TC_TIPTILT_STAGE_HOME [OK] */
    5, /* command 111 : TC_TIPTILT_STAGE_POSITION [OK] */
    0, /* command 112 */
    0, /* command 113 : TC_ROTATION_CENTER */
    0, /* command 114 : TC_PLATE_SCALE */
    0, /* command 115 : TC_TRACKSTAR_NUMBER */
    0, /* command 116 : TC_STACK_SIZE_ID*/
    5, /* command 117 : TC_SOLVE_FIELD [OK] */
    0, /* command 118 */
    0, /* command 119 */
    1, /* command 120 : TC_TIPTILT_STAGE_2_HOME [OK] */
    5, /* command 121 : TC_TIPTILT_STAGE_2_POSITION [OK] */
    0, /* command 122 */
    0, /* command 123 */
    0, /* command 124 */
    0, /* command 125 */
    0, /* command 126 */
    0, /* command 127 */
    0, /* command 128 */
    0, /* command 129 */
    1, /* command 130 : TC_TIPTILT_STAGE_3_HOME [OK] */
    5, /* command 131 : TC_TIPTILT_STAGE_3_POSITION [OK] */
    6, /* command 132 : TC_TIPTILT_FOCUS [OK] */
    1, /* command 133 : TC_TIPTILT_ABCMOVES    */ //ICA -- trying out ABCMoves
    0, /* command 134 */
    2, /* command 135 : TC_CAROUSEL_POS [OK] */
    1, /* command 136 : TC_NOMINAL_FOCUS [OK] */
    5, /* command 137 : TC_FOCUS_DELTA [OK] */
    0, /* command 138 */
    0, /* command 139 */
    2, /* command 140 : TC_SHUTDOWN_GUIDER [OK] */
    0, /* command 141 */
    0, /* command 142 */
    0, /* command 143 */
    0, /* command 144 */
    0, /* command 145 */
    0, /* command 146 */
    0, /* command 147 */
    0, /* command 148 */
    0, /* command 149 */
    2, /* command 150 : TC_STAR_BOX_CUT [OK] */
    0, /* command 151 */
    0, /* command 152 */
    0, /* command 153 */
    0, /* command 154 */
    0, /* command 155 */
    0, /* command 156 */
    0, /* command 157 */
    0, /* command 158 */
    0, /* command 159 */
    0, /* command 160 */
    0, /* command 161 */
    0, /* command 162 */
    0, /* command 163 */
    0, /* command 164 */
    0, /* command 165 */
    0, /* command 166 */
    0, /* command 167 */
    0, /* command 168 */
    0, /* command 169 */
    0, /* command 170 */
    0, /* command 171 */
    0, /* command 172 */
    0, /* command 173 */
    0, /* command 174 */
    0, /* command 175 */
    0, /* command 176 */
    0, /* command 177 */
    0, /* command 178 */
    0, /* command 179 */
    0, /* command 180 */
    0, /* command 181 */
    0, /* command 182 */
    0, /* command 183 */
    0, /* command 184 */
    0, /* command 185 */
    0, /* command 186 */
    0, /* command 187 */
    0, /* command 188 */
    0, /* command 189 */
    0, /* command 190 */
    0, /* command 191 */
    0, /* command 192 */
    0, /* command 193 */
    0, /* command 194 */
    0, /* command 195 */
    0, /* command 196 */
    0, /* command 197 */
    0, /* command 198 */
    0, /* command 199 */
    0, /* command 200 : TC_FIELD_SOLUTION [OK] - this is TM*/
    0, /* command 201 */
    0, /* command 202 */
    0, /* command 203 */
    0, /* command 204 */
    0, /* command 205 */
    0, /* command 206 */
    0, /* command 207 */
    0, /* command 208 */
    0, /* command 209 */
    0, /* command 210 */
    0, /* command 211 */
    0, /* command 212 */
    0, /* command 213 */
    0, /* command 214 */
    0, /* command 215 */
    0, /* command 216 */
    0, /* command 217 */
    0, /* command 218 */
    0, /* command 219 */
    0, /* command 220 */
    5, /* command 221 : TC_SET_FB_GAIN [OK] */
    3, /* command 222 : TC_SET_FB_SATVAL [OK] */
    5, /* command 223 : TC_SET_FB_THRESH [OK] */
    3, /* command 224 : TC_SET_FB_DISTOL [OK] */
    3, /* command 225 : TC_SET_FB_GRIDSIZE [OK] */
    3, /* command 226 : TC_SET_FB_CENBOX [OK] */
    3, /* command 227 : TC_SET_FB_APBOX [OK] */
    5, /* command 228 : TC_SET_FB_OFFSET [OK] */
    5, /* command 229 : TC_SET_FB_RN [OK] */
    1, /* command 230 : TC_RESET_FB [OK] */
    0, /* command 231 */
    0, /* command 232 */
    0, /* command 233 */
    0, /* command 234 */
    0, /* command 235 */
    0, /* command 236 */
    0, /* command 237 */
    0, /* command 238 */
    0, /* command 239 */
    0, /* command 240 */
    0, /* command 241 */
    0, /* command 242 */
    0, /* command 243 */
    0, /* command 244 */
    0, /* command 245 */
    0, /* command 246 */
    0, /* command 247 */
    0, /* command 248 : TC_STAGE_STATUS - this is TM */
    0, /* command 249 */
    0, /* command 250 */
    0, /* command 251 */
    0, /* command 252 */
    0, /* command 253 */
    0, /* command 254 */
    0 /* command 255 */
  };

  //frameblob calls
  tc_set_fb_gridsize_t tc_set_fb_gridsize;
  tc_set_fb_cenbox_t tc_set_fb_cenbox;
  tc_set_fb_apbox_t tc_set_fb_apbox;
  
  tc_set_fb_gain_t tc_set_fb_gain;
  tc_set_fb_offset_t tc_set_fb_offset;
  tc_set_fb_rn_t tc_set_fb_rn;
  tc_set_fb_thresh_t tc_set_fb_thresh;

  tc_set_fb_satval_t tc_set_fb_satval;
  tc_set_fb_distol_t tc_set_fb_distol;

  tc_set_guider_subtract_t tc_set_guider_subtract; 

  unsigned short dct;

  // add to the received bytes to the  message buffer
  
  

  int push_ground_message(char *msg, int size){
    // /* REMOVE STUFF*/
    // static int initialized=0;
    // static FILE *fptr;
    // if (!initialized){
    //   initialized=1;
    //   fptr = fopen("guider.out","ab");      
    // };
    // fwrite(msg,size,1,fptr);
    // fflush(fptr);
    // /* REMOVE STUFF */
    return gbuffer.push(msg, size);
  }; // push_ground_message

  // process the first message in the queue, if there is one. 
  int process_ground_message(){
    static int length,i;
    static unsigned short mlen;
    static unsigned char msg[512];
    static char str[64];
    static int buffercounter=0;
    // only do something if buffer is not empty
    if(!gbuffer.isEmpty()){
      DEBUGLINE;
      //      for(i = 0; i< gbuffer.used; i++)
      //      	printf(" %d ",gbuffer[i]);
      //      printf("++++++++++++++++++==\n");
      // strip fluff
      while (gbuffer.used > 0 && gbuffer[0] != 0x55)
	gbuffer.strip(1);
      // find first message byte
      for(i=0;i<gbuffer.used;i++){
	// find a possible message start
	if(gbuffer[i] == 0x55){
	  // get rid of everything before this, and reset the loop counter
	  //	  gbuffer.strip(i); i=0;
	  // is there enough for a message to exist? 
	  if(gbuffer.used > 5){
	    // what is the reported  message length? 
	    mlen = 256*gbuffer[4]+gbuffer[3];
	    DEBUGPRINT("length : %u\n",mlen);
	    // compare with the message length for this message ID
	    if( ( mlen!=ground_message_length[(unsigned char)gbuffer[5]]) || ( mlen > 100  ) ){
	      sprintf(str,"MID/Length Err %d %d ( != %d)",(unsigned char) gbuffer[5],mlen,ground_message_length[(unsigned char)gbuffer[5]]);
	      fblog::logerr("Bad MID or Length: ctr=%d id=%d len=%d",
			      256*int(gbuffer[2])+gbuffer[1], gbuffer[5],mlen);
	      ground::send_text(str);
	      // invalid, message so let's move on.
	      gbuffer.strip(1);
	      return 0;
	      // if there is a mismatch, this cannot be a valid 
	      // message, so strip away the leading character
	      // and wait to process at next loop
	      //DEBUGPRINT("YOYO");
	      //gbuffer.strip(1);
	      //return 0;
	    }; // length mismatch
	    // do we have enough bytes for this message? 
	    DEBUGPRINT("buffer.used %d\n",gbuffer.used);
	    if(mlen+7 > gbuffer.used){
	      // if not, just exit and wait for more data.
	      return 0;
	    }; // buffer too short for valid message
	    // is the terminating character there? 
	    if(gbuffer[6+mlen] == -86){ // unsigned vs. signed buffer
	      DEBUGLINE;
	      DEBUGPRINT("buff %d %d\n",FindCRC(&gbuffer,7+mlen), (unsigned char) gbuffer[mlen+5]);
	      if( FindCRC(&gbuffer,7+mlen) == (unsigned char) gbuffer[mlen+5] ){
		length = 7+mlen;
		gbuffer.pop(length,(char *)msg);
		buffercounter=0;
		return interpret_ground_message(&(msg[0]), length);
	      } else {
		DEBUGLINE;
		gbuffer.strip(1);
		return 0;
	      }; // VALID
	    } else {
	      DEBUGLINE;
	      gbuffer.strip(1);
	      return 0;
	    }; // terminating character
	  }; // ring.used > 5
	  
	} else {
	  DEBUGLINE;
	  if ( buffercounter > MAXBUFFERCOUNTER) {  
	    static char tmpmsg[32];
	    sprintf(tmpmsg,"BUFFER COUNTER!!!");
	    send_text(tmpmsg);
	    buffercounter=0;
	    gbuffer.strip(1);
	    
	  } else { 
	    buffercounter++;
	  };
	}; // 0x55
      }; // for i=0; i<buffer.used
    } else {
      return 0;
    }; // buffer.used...
    return 0;
  }; // extract_ground_message

	
  // CRC functions
  unsigned char FindCRC(RingBuffer *rb, int length){
    static unsigned  char crc;
    static int i;
    crc=(*rb)[1];
    for(i=2;i<length-2;i++){
      crc = crc ^ ((*rb)[i]);
    }; // for loop
   return crc;
  }; // FindCRC(Ringbuffer)

  unsigned char FindCRC(unsigned char *mg, int length){
    static unsigned char crc;
    static int i;
    crc = mg[1];
    for(i=2;i<length-2;i++) crc = crc ^ mg[i];
    return crc;
  }; // FindCRC(unsigned char)
  
  
  // message processing function
  // assumes a VALID message
  int interpret_ground_message(unsigned char *msg, int length){
    static unsigned char mID;
    static char tmpmsg[255];
    
    // what is the message ID?
    mID = msg[5];
    send_echo(mID);
    printf("Sending echo!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    // send an echo
    DEBUGPRINT("mID = %u\n",mID);
    // do something for each message
    switch(mID){
    case TC_SET_EXPOSURE_TIME_ID: // 4
      tc_set_exposure_time = *((tc_set_exposure_time_t *) msg);
      request_new_exptime(tc_set_exposure_time.var_uint);
      DEBUGPRINT("[%d] Exposure time: %d\n",TC_SET_EXPOSURE_TIME_ID,tc_set_exposure_time.var_uint);
      // call for camera to change exposure time goes here
      break;
    case TC_SET_DISPLAY_MODE_ID: 
      tc_set_display_mode = *((tc_set_display_mode_t *) msg);
      fbvideo::change_display_mode(tc_set_display_mode.dispmode);
      DEBUGPRINT("[%d] Display mode: %d\n",TC_SET_DISPLAY_MODE_ID,tc_set_display_mode.dispmode);
      break;
    case TC_SET_GUIDER_MODE_ID: // 102
      tc_set_guider_mode = *((tc_set_guider_mode_t *) msg);
      img::setMaxStars(MAXSTARS);
      img::setFBMode((FBModes) tc_set_guider_mode.gmode);
      DEBUGPRINT("[%d] Guider mode: %d\n",TC_SET_GUIDER_MODE_ID,tc_set_guider_mode.gmode);
      break;
    case TC_SET_GUIDER_MODE_ACQUIRE_LIMITED_ID: // 
      tc_set_guider_mode_acquire_limited = *((tc_set_guider_mode_acquire_limited_t *)msg);
      img::setMaxStars(tc_set_guider_mode_acquire_limited.value);
      img::setFBMode((FBModes) 1);
      DEBUGPRINT("[%d] Guide mode: %d; Stars: %d\n",1,tc_set_guider_mode_acquire_limited.value);
      break;
    case TC_SET_TARGET_POSITIONS_ID: //103
      tc_set_target_positions = *((tc_set_target_positions_t *) msg);
      img::set_target_points(tc_set_target_positions.x,
			     tc_set_target_positions.y);
      for(int jdx=0;jdx<MAXSTARS;jdx++){
	DEBUGPRINT("[%d] Tgt %d %d %d\n",TC_SET_TARGET_POSITIONS_ID,jdx,tc_set_target_positions.x[jdx],tc_set_target_positions.y[jdx]);
      }; // jdx
      // lots of target positions sent from the ground.
      break;
    case TC_LOAD_TARGET_ID:
      tc_load_target = *((tc_load_target_t *) msg);
      img::load_next_target(tc_load_target.targetid);
      DEBUGPRINT("[%d] Load target: %d\n",TC_LOAD_TARGET_ID,
		 tc_load_target.targetid);
      break;
    case TC_SELECT_CENTROIDING_ALGORITHM_ID:
      tc_select_centroiding_algorithm = *((tc_select_centroiding_algorithm_t *)msg);
      img::set_centroiding_algorithm(tc_select_centroiding_algorithm.value);
      printf("----> CENTROIDING ALGORITHM");
      printf("ALG %d\n",img::get_centroiding_algorithm());
      fblog::logmsg("ALG %d",img::get_centroiding_algorithm());
      break;
    case TC_ADJUST_POINTING_ID:
      tc_adjust_pointing =*((tc_adjust_pointing_t *) msg);
      img::adjust_targets(tc_adjust_pointing.dx, tc_adjust_pointing.dy, tc_adjust_pointing.drot);
      img::update_targets();
      DEBUGPRINT("[%d] Adjust Pointing: %f %f %f\n",TC_ADJUST_POINTING_ID,
		 tc_adjust_pointing.dx,
		 tc_adjust_pointing.dy,
		 tc_adjust_pointing.drot);
      break;
    case TC_ADJUST_SINGLE_STAR_ID:
      tc_adjust_single_star = *((tc_adjust_single_star_t *)msg);
      img::adjust_single_star(tc_adjust_single_star.starid, 
			      tc_adjust_single_star.newx,
			      tc_adjust_single_star.newy);
      break;
    case TC_SWITCH_STAR_ID: 
      tc_switch_star = *((tc_switch_star_t *)msg);
      img::set_next_single(tc_switch_star.x,tc_switch_star.y,1);
      DEBUGPRINT("[%d] Switch star: %f %f\n",TC_SWITCH_STAR_ID,
		 tc_switch_star.x,
		 tc_switch_star.y);
      break;

    case TC_SWITCH_TO_NEXT_ID: 
      tc_switch_to_next = *((tc_switch_to_next_t *) msg);
      if(tc_switch_to_next.switch_flag == 0) img::set_change_flag(1,0,0,0);
      if(tc_switch_to_next.switch_flag == 1) img::set_change_flag(0,1,0,0);
      if(tc_switch_to_next.switch_flag == 2) img::set_change_flag(1,1,0,0);
      DEBUGPRINT("[%d] switch to next: %d\n",TC_SWITCH_TO_NEXT_ID,
		 tc_switch_to_next.switch_flag);
      break;

      // 120727 -- matmat
    case TC_CHANGE_USED_STARS_ID:
      tc_change_used_stars = *((tc_change_used_stars_t *) msg);
      img::change_used_stars(tc_change_used_stars.usemask);
      printf("[%d] Change Used Stars %u\n",
		 TC_CHANGE_USED_STARS_ID,
		 tc_change_used_stars.usemask);
      break;

    case TC_ADJUST_SIGMA_MINMAX_ID:
      tc_change_guide_star_sigma = *((tc_change_guide_star_sigma_t *) msg);
      img::change_guide_star_sigma(tc_change_guide_star_sigma.thresh, tc_change_guide_star_sigma.sigmin, tc_change_guide_star_sigma.sigmax);
      DEBUGPRINT("[%d] Set Guide Star Sigma %u %u %u\n",
		 TC_ADJUST_SIGMA_MINMAX_ID,
		 tc_change_guide_star_sigma.thresh,
		 tc_change_guide_star_sigma.sigmin,
		 tc_change_guide_star_sigma.sigmax);
      break;

    case TC_CONVERT_NEXT_TARGET_TO_STAR_ID:
      tc_convert_next_target_to_star = *((tc_convert_next_target_to_star_t *)msg);
      img::convert_next_target_to_next_star(tc_convert_next_target_to_star.dx,
					    tc_convert_next_target_to_star.dy,
					    tc_convert_next_target_to_star.dr);
      DEBUGPRINT("[%d] T-->S: %f %f %f\n",TC_CONVERT_NEXT_TARGET_TO_STAR_ID,
		 tc_convert_next_target_to_star.dx,
		 tc_convert_next_target_to_star.dy,
		 tc_convert_next_target_to_star.dr);
      break; 

    case TC_CONVERT_NEXT_TARGET_TO_STAR_SKY_ID:
      /// FIX THIS
      img::convert_next_target_to_next_star_sky(-100.0d,-100.0d,-1.01);
      DEBUGPRINT("[%d] Convert target to star using to sky\n",
		 TC_CONVERT_NEXT_TARGET_TO_STAR_SKY_ID);
      break;

    case TC_SOLVE_FIELD_ID:
      tc_solve_field = *((tc_solve_field_t *)msg);
      request_new_stack(tc_solve_field.stack,
			tc_solve_field.stacksize,
			tc_solve_field.solve);
      DEBUGPRINT("[%d] Solve field: %d %d %d\n",TC_SOLVE_FIELD_ID,
		 tc_solve_field.stack,
		 tc_solve_field.stacksize,
		 tc_solve_field.solve);
      break;

    case TC_SHUTDOWN_GUIDER_ID: // 142
      tc_shutdown_guider = *((tc_shutdown_guider_t *)msg);
      fblog::logmsg("TC142: Shutdown guider received with type %d", tc_shutdown_guider.value );
      if (tc_shutdown_guider.value == 1){
	fblog::logmsg("I would power down the computer");
      } else if (tc_shutdown_guider.value == 2){
	fblog::logmsg("I would restart the computer");
      } else {
	fblog::logmsg("I'd sit there confused!");
      };
      break;

      // 104: TC_SET_DISPLAY_STRETCH
    case TC_SET_DISPLAY_STRETCH_ID: 
      tc_set_display_stretch = *((tc_set_display_stretch_t *) msg);
      DEBUGPRINT("[%d] Set Display Stretch %d %d %f\n",
		 TC_SET_DISPLAY_STRETCH_ID,
		 tc_set_display_stretch.min,
		 tc_set_display_stretch.max,
		 tc_set_display_stretch.gamma);

      if(tc_set_display_stretch.min >= 0){
	fbvideo::change_stretch(tc_set_display_stretch.min,
				tc_set_display_stretch.max,
				tc_set_display_stretch.gamma);
      } else {
	fbvideo::reset_stretch();
      }; // TC_SET_DISPLAY_STRETCH
      break;
    case TC_STAR_BOX_CUT_ID:
      static FBModes curmode;
      tc_star_box_cut = *((tc_star_box_cut_t *) msg);
      fprintf(stderr,"TC_STAR_BOX_CUT %d\n",tc_star_box_cut.value);
      sprintf(tmpmsg,"Set starboxcut to: %d",tc_star_box_cut.value);
      curmode=img::getFBMode();
      img::setFBMode((FBModes) 0);
      usleep(100000);
      img::boxes_redo_star_cut( (int) tc_star_box_cut.value);
      img::setFBMode((FBModes) curmode);
      usleep(100000);
      ground::send_text(tmpmsg);
      break;
    case TC_MASK_STAGE_HOME_ID:
      motors::home(ROTAXIS);
      break;
    case TC_TIPTILT_STAGE_HOME_ID:
      motors::home(LINAAXIS);
      break;
    case TC_TIPTILT_STAGE_2_HOME_ID:
      motors::home(LINBAXIS);
      break;
    case TC_TIPTILT_STAGE_3_HOME_ID:
      motors::home(LINCAXIS);
      break;
    case TC_TIPTILT_FOCUS_ID:
      tc_set_focus_delay = *((tc_set_focus_delay_t *)msg);
      motors::start_focus((char)tc_set_focus_delay.delay, tc_set_focus_delay.step);
      break;

      //########################
      //testing new function

    case TC_TIPTILT_ABCMOVES_ID:
      fprintf(stderr,"Received ABC Moves\n");
      motors::start_abc();
      break;

      //#######################
    case TC_FOCUS_DELTA_ID:
      tc_focus_delta = *((tc_focus_delta_t *)msg);
      motors::focus_delta(tc_focus_delta.var_float);
      break;
    case TC_NOMINAL_FOCUS_ID:
      fprintf(stderr,"Received nominal focus\n");
      motors::nominal();
      break;
    case TC_SET_IMAGE_TAG_ID:
      tc_set_image_tag = *((tc_set_image_tag_t *) msg);
      img::set_image_tag(tc_set_image_tag.var_ushort);
      break;
    case TC_SET_IMAGE_SAVE_PERIOD_ID:
      tc_set_image_save_period = *((tc_set_image_save_period_t *) msg);
      img::set_image_save_period(tc_set_image_save_period.var_ushort);
      break;
    case TC_REFINE_TARGETS_ID:
      tc_refine_targets=*((tc_refine_targets_t *)msg);
      img::start_refine_targets(tc_refine_targets.value);
      break;
    case TC_COLLECT_MIN_IMAGE_ID:
      tc_compute_min_image=*((tc_compute_min_image_t *)msg);
      img::start_construct_min_image(tc_compute_min_image.value);
      break;
    case TC_CAROUSEL_POS_ID:
      tc_carousel_pos = *((tc_carousel_pos_t *) msg);
      fprintf(stderr,"Carousel position received\n");
      motors::set_carousel_station(tc_carousel_pos.value);
      break;
    case TC_MASK_STAGE_POSITION_ID:
      tc_mask_stage_position = *((tc_mask_stage_position_t *) msg);
      motors::absmove(ROTAXIS, tc_mask_stage_position.var_float);
      break;
    case TC_TIPTILT_STAGE_POSITION_ID:
      tc_tiptilt_stage_position = *((tc_tiptilt_stage_position_t *) msg);
      motors::absmove(LINAAXIS, tc_tiptilt_stage_position.var_float);
      break;
    case TC_TIPTILT_STAGE_2_POSITION_ID:
      tc_tiptilt_stage_position = *((tc_tiptilt_stage_position_t *) msg);
      motors::absmove(LINBAXIS, tc_tiptilt_stage_position.var_float);
      break;
    case TC_TIPTILT_STAGE_3_POSITION_ID:
      tc_tiptilt_stage_position = *((tc_tiptilt_stage_position_t *) msg);
      motors::absmove(LINCAXIS, tc_tiptilt_stage_position.var_float);
      break;
    case TC_SET_TTL_ON_ID:
      tc_set_ttl = *((tc_set_ttl_t *) msg);
      //      dct = ((unsigned short) tc_set_ttl.ttl_id);
      dct = ((unsigned short) tc_set_ttl.var_ushort);
      adio::digital_on(dct);
      break;
    case TC_SET_TTL_OFF_ID:
      tc_set_ttl = *((tc_set_ttl_t *) msg);
      //dct = ((unsigned short) tc_set_ttl.ttl_id);
      dct = ((unsigned short) tc_set_ttl.var_ushort);
      adio::digital_off(dct);
      break;
    case TC_SET_GUIDER_SUBTRACT_ID:
      // do we want to be subtracting median (1) or rows (2) or both (3);
      tc_set_guider_subtract = *((tc_set_guider_subtract_t*) msg);
      img::set_subtract(tc_set_guider_subtract.value);
      fblog::logmsg("IMGSUB %d",img::get_subtract());
      break;
    case TC_SET_FB_SATVAL_ID:
      tc_set_fb_satval = *((tc_set_fb_satval_t *)msg);
      fblog::logmsg("FB_SATVAL %d",tc_set_fb_satval.var_ushort);
      img::set_fb_satval(tc_set_fb_satval.var_ushort);
      break;
    case TC_SET_FB_GRIDSIZE_ID:
      tc_set_fb_gridsize = *((tc_set_fb_gridsize_t *)msg);
      fblog::logmsg("FB_GRID %d",tc_set_fb_gridsize.var_ushort);
      img::set_fb_gridsize(tc_set_fb_gridsize.var_ushort);
      break;
    case TC_SET_FB_CENBOX_ID:
      tc_set_fb_cenbox = *((tc_set_fb_cenbox_t *)msg);
      fblog::logmsg("FB_CENBOX %d",tc_set_fb_cenbox.var_ushort);
      img::set_fb_cenbox(tc_set_fb_cenbox.var_ushort);
      break;
    case TC_SET_FB_APBOX_ID:
      tc_set_fb_apbox = *((tc_set_fb_apbox_t *)msg);
      fblog::logmsg("FB_APBOX %d",tc_set_fb_apbox.var_ushort);
      img::set_fb_apbox(tc_set_fb_apbox.var_ushort);
      break;
    case TC_SET_FB_THRESH_ID:
      tc_set_fb_thresh = *((tc_set_fb_thresh_t *)msg);
      fblog::logmsg("FB_THRESH %f",tc_set_fb_thresh.var_float);
      img::set_fb_thresh(tc_set_fb_thresh.var_float);
      break;
    case TC_SET_FB_GAIN_ID:
      tc_set_fb_gain = *((tc_set_fb_gain_t *)msg);
      fblog::logmsg("FB_GAIN %f",tc_set_fb_gain.var_float);
      img::set_fb_gain(tc_set_fb_gain.var_float);
      break;
    case TC_SET_FB_OFFSET_ID:
      tc_set_fb_offset = *((tc_set_fb_offset_t *)msg);
      fblog::logmsg("FB_OFFSET %f",tc_set_fb_offset.var_float);
      img::set_fb_offset(tc_set_fb_offset.var_float);
      break;
    case TC_SET_FB_RN_ID:
      tc_set_fb_rn = *((tc_set_fb_rn_t *)msg);
      fblog::logmsg("FB_RN %f",tc_set_fb_rn.var_float);
      img::set_fb_rn(tc_set_fb_rn.var_float);
      break;
    case TC_SET_FB_DISTOL_ID:
      tc_set_fb_distol = *((tc_set_fb_distol_t *)msg);
      fblog::logmsg("FB_DISTOL %s", tc_set_fb_distol.var_short);
      img::set_fb_distol(tc_set_fb_distol.var_short);
      break;
    case TC_RESET_FB_ID:
      fblog::logmsg("Resetting to frameblob defaults");
      img::reset_fb();
      break;
    case TC_DITHER_ID:
      tc_dither = *((tc_dither_t *)msg);
      fblog::logmsg("dither rcvd %d %d",(char)tc_dither.delay, tc_dither.pattern);
      motors::start_dither((char)tc_dither.delay, tc_dither.pattern);
      break;
    default:
      break;

    }; // switch
    
    return 0;
  }; // process_ground_message


  // command specific handling functions
  void send_status(){
    return;
  };  // send_status

  void send_video_status(){
    return;
  }; //send_video_status


  void send_echo(unsigned char mID){
    static tm_echo_command_t tmec;
    tmec.ssync = 0x55;
    tmec.esync = 0xAA;
    tmec.count = GROUND_COUNTER();
    tmec.length = 2;
    tmec.mid = TC_COMMAND_ECHO_ID;
    tmec.id = mID;;
    tmec.crc = COMPUTE_GROUND_CRC(tmec);
    DEBUGPRINT("%d\n",mID);
    DEBUGPRINT("size=%d\n",sizeof(tmec));
    WRITE_TO_GROUND(tmec);
    return;
  }; // send_echo

  // this one is a little different, since it can be varying length...
  // plus it is hard coded, for now. 
  void send_text(char *str){
    static unsigned short len;
    static int intlen;
    static unsigned char message[255];
    static unsigned short *us;

    len = strlen(str);
    intlen=len;
    message[0]=0x55;
    us = (unsigned short *)(message+1);
    *us = GROUND_COUNTER();
    us = (unsigned short *)(message+3);
    *us = len+1;
    message[5]=124;
    memcpy(message+6,str,len);
    message[len+6]=FindCRC(message,intlen+7);
    message[len+7]=0xAA;

    DEBUGPRINT("SEND_TEXT %d\n",len);

    write_to_ground( (char*) message,intlen+8);

    return;
  };
  
  void send_dtu_radec(){
    static tm_dtu_radec_t tmrd;
    static double ra,dec,roll;
    tmrd.ssync = 0x55;
    tmrd.esync = 0xAA;
    tmrd.count = GROUND_COUNTER();
    gondola::get_gondola_radec(&ra,&dec,&roll);
    tmrd.ra = (float) ra;
    tmrd.dec = (float) dec;
    tmrd.roll = (float) roll;
    tmrd.mid = TM_DTU_RA_DEC_ID;
    tmrd.length = TM_DTU_RA_DEC_LENGTH;
    tmrd.crc = COMPUTE_GROUND_CRC(tmrd);
    WRITE_TO_GROUND(tmrd);
    return;
  };// send_dtu_radec

  void send_gondola_boresight(){
    static tm_gondola_boresight_t tmgb;
    tmgb.ssync = 0x55;
    tmgb.esync = 0xAA;
    tmgb.count = GROUND_COUNTER();
    gondola::get_gondola_boresight(&tmgb.az,&tmgb.el, &tmgb.rot);
    tmgb.length=TM_GONDOLA_BORESIGHT_LENGTH;
    tmgb.mid = TM_GONDOLA_BORESIGHT_ID;
    tmgb.crc = COMPUTE_GROUND_CRC(tmgb);
    WRITE_TO_GROUND(tmgb);
    return;
  };

  void send_stage_status(){
    static tm_stage_status_t tmss;
    tmss.ssync = 0x55;
    tmss.esync = 0xAA;
    tmss.count = GROUND_COUNTER();
    tmss.length = (unsigned char)(sizeof(tmss)-7);
    tmss.mid = TC_STAGE_STATUS_ID;
    tmss.stagerpos = motors::get_motorpos(ROTAXIS);
    tmss.stage1pos = motors::get_motorpos(LINAAXIS);
    tmss.stage2pos = motors::get_motorpos(LINBAXIS);
    tmss.stage3pos = motors::get_motorpos(LINCAXIS);
    motors::get_stage_statuses(&tmss.stageerr[0], &tmss.poserr[0], &tmss.stagestate[0]);
    DEBUGPRINT("STAGE %f %f %f %f\n",tmss.stagerpos,tmss.stage1pos,tmss.stage2pos,tmss.stage3pos);
    tmss.crc = COMPUTE_GROUND_CRC(tmss);
    WRITE_TO_GROUND(tmss);
  }; // send_stage_status

  /*
  void send_errors (){
    static tm_guider_errors_t tmge;
    tmge.ssync = 0x55;
    tmge.esync = 0xAA;
    tmge,count = GROUND_COUNTER();
    tmge.length=9;
    //img:getGuidererrors(
    //tmge.mid = TM
  };
  */
  
  void send_dio_status(){
    static tm_dio_status_t tmds;
    static unsigned char dio;
    tmds.ssync = 0x55;
    tmds.esync = 0xAA;
    tmds.count = GROUND_COUNTER();
    tmds.mid = TM_DIO_STATUS_ID;
    tmds.length = 2;
    adio::digital_in(&dio);
    tmds.dio = dio;
    tmds.crc = COMPUTE_GROUND_CRC(tmds);
    WRITE_TO_GROUND(tmds);
    DEBUGPRINT("TMDS\n");
    //    printf("Sending dio status.");
    return;
  };

   void send_guider_status(){
    static tm_guider_status_t tmgs;
    tmgs.ssync = 0x55;
    tmgs.esync = 0xAA;
    tmgs.count = GROUND_COUNTER();
    //tmgs.length = TM_GUIDER_STATUS_LENGTH;
    tmgs.length = (unsigned char) (sizeof(tmgs)-7);
    tmgs.mid = TM_GUIDER_STATUS_ID;
    tmgs.imgtag = img::get_image_tag();
    tmgs.guidemode = (char) img::getFBMode(); 
    tmgs.dispmode = (char) fbvideo::get_display_mode();
    tmgs.algorithm = img::get_centroiding_algorithm();
    tmgs.subtract = img::get_subtract();
    tmgs.saveperiod = img::get_image_save_period();
    tmgs.frameno = img::getFramenumber();
    retrieve_exptime(&(tmgs.exptime));
    retrieve_temperatures( &(tmgs.ccdtemp), &(tmgs.camtemp), &(tmgs.pstemp));
    adio::retrieve_pt_multi(&(tmgs.temperature),&(tmgs.pressure),
      &(tmgs.gobc_temp),&(tmgs.gobc_pressure),&(tmgs.cal_temp),&(tmgs.cal_pressure));
    tmgs.crc = COMPUTE_GROUND_CRC(tmgs);
    //printf("TMGS GOBC T: %f %f\n",tmgs.gobc_temp,tmgs.gobc_pressure);
    DEBUGPRINT("TMGS %d %d\n",sizeof(tmgs),sizeof(unsigned long int));
    //printf("TMGS %c\n", tmgs);
    WRITE_TO_GROUND(tmgs);
    return;
   };// send_guider_status()

  // 170727 -- matmat
  void send_target_info(){
    static tm_targets_t tmti;
    static float xarr[MAXSTARS],yarr[MAXSTARS];
    static int idx;
    tmti.ssync = 0x55;
    tmti.esync = 0xAA;
    tmti.count = GROUND_COUNTER();
    tmti.length = (unsigned char) (sizeof(tmti)-7);
    tmti.mid = TM_TARGETS_ID;
    img::get_target_pointers(xarr,yarr);
    
    //memcpy(xarr,tx,MAXSTARS*sizeof(float));
    //memcpy(yarr,ty,MAXSTARS*sizeof(float));
    for (idx=0;idx<MAXSTARS;idx++){
      tmti.coords[2*idx]= floor(xarr[idx]*50.0);
      tmti.coords[2*idx+1]=floor(yarr[idx]*50.0);	
    }; //idx
    img::get_thrsig( &(tmti.snrcut),&(tmti.smin), &(tmti.smax));
    tmti.crc = COMPUTE_GROUND_CRC(tmti);
    WRITE_TO_GROUND(tmti);
    DEBUGPRINT("TMTI %d\n",sizeof(tmti));
    return;
  };

  // 170727 -- matmat
  void send_star_info(){
    static tm_stars_t tmsi;
    static float xarr[MAXSTARS],yarr[MAXSTARS],farr[MAXSTARS];
    static float sigx[MAXSTARS],sigy[MAXSTARS];
    static int idx;
    static unsigned char mult;
    static unsigned char use[MAXSTARS], valid[MAXSTARS];
    tmsi.ssync = 0x55;
    tmsi.esync = 0xAA;
    tmsi.count = GROUND_COUNTER();
    tmsi.length = (unsigned char) (sizeof(tmsi)-7);
    tmsi.mid = TM_STARS_ID;
    img::get_star_pointers(xarr,yarr,farr,sigx,sigy);
    img::get_star_usevalid(use, valid);

    mult=1;
    tmsi.use=tmsi.valid=0;
    for (idx=0;idx<MAXSTARS;idx++){
      tmsi.coords[3*idx]= floor(xarr[idx]*50.0);
      tmsi.coords[3*idx+1]=floor(yarr[idx]*50.0);	
      tmsi.coords[3*idx+2]=floor(farr[idx]);
      if (sigx[idx] > 12.5) sigx[idx]=0.0;
      if (sigy[idx] > 12.5) sigy[idx]=0.0;
      tmsi.sigmas[2*idx] = floor(sigx[idx]*20.0);
      tmsi.sigmas[2*idx+1] = floor(sigy[idx]*20.0);
      tmsi.use +=   use[idx]*mult;
      tmsi.valid +=  valid[idx]*mult;
      mult=mult*2;

    }; //idx
    tmsi.crc = COMPUTE_GROUND_CRC(tmsi);
    WRITE_TO_GROUND(tmsi);
    DEBUGPRINT("TMSI %d\n",sizeof(tmsi));
    return;
  };

  void send_fb_info(){
    tm_fb_info_t tmfi;
    static unsigned short fbsatval;
    static unsigned int fbgridsize, fbcenbox, fbapbox;
    static float fbgain, fbthresh, fboffset, fbrn;
    static int fbdistol;
    // pull the values. 
    img::get_fb_vals(&fbgain, &fbsatval, &fbthresh,
		     &fbdistol, &fbgridsize, &fbcenbox,
		     &fbapbox, &fboffset, &fbrn);
    // fill structure
    tmfi.ssync = 0x55;
    tmfi.esync = 0xAA;
    tmfi.count = GROUND_COUNTER();
    tmfi.length = (unsigned char) (sizeof(tmfi)-7);
    tmfi.mid = TM_FB_INFO_ID;
    tmfi.gain = (unsigned short) round(fbgain * 2048);
    tmfi.satval = fbsatval;
    tmfi.thresh = (unsigned short) round(fbthresh * 2048);
    tmfi.distol = ( short ) fbdistol;
    tmfi.gridsize = (unsigned short) fbgridsize;
    tmfi.cenbox = ( unsigned short) fbcenbox;
    tmfi.apbox = (unsigned short) fbapbox;
    tmfi.offset = (short ) round(fboffset);
    tmfi.rn = (unsigned short) round(fbrn * 2048);
    tmfi.crc= COMPUTE_GROUND_CRC(tmfi);
    WRITE_TO_GROUND(tmfi);
    DEBUGPRINT("TMFI %d\n",sizeof(tmfi));
  }; //send_fb_info

  void send_guide_stats(){
    static tm_guide_stats_t tmgs;
    static float dx,dy,dr,rmsdx,rmsdy,rmsdr,msx,msy;
    img::get_guide_stats(&dx,&dy,&dr,&rmsdx,&rmsdy,&rmsdr,&msx,&msy);
    tmgs.ssync = 0x55;
    tmgs.esync = 0xAA;
    tmgs.count = GROUND_COUNTER();
    tmgs.length = (unsigned char) (sizeof(tmgs)-7);
    tmgs.mid = TM_GUIDE_STATS_ID;
    if (isnan(dx)) {
      dx=0.0;
      rmsdx=-1.0;
    };
    if (isnan(rmsdx)) rmsdx=-1.0;
    if (isnan(dy)) {
      dy=0.0;
      rmsdy=-1.0;
    };
    if (isnan(rmsdy)) rmsdy=-1.0;
    if (isnan(dr)) {
      dr=0.0;
      rmsdr=-1.0;
    };
    if (isnan(rmsdr)) rmsdr=-1.0;
    if (isnan(msx)) msx=0.0;
    if (isnan(msy)) msy=0.0;
    tmgs.dx=dx;
    tmgs.dy=dy;
    tmgs.dr=dr;
    tmgs.rmsdx=rmsdx;
    tmgs.rmsdy=rmsdy;
    tmgs.rmsdr=rmsdr;
    tmgs.msx=msx;
    tmgs.msy=msy;
    tmgs.crc= COMPUTE_GROUND_CRC(tmgs);
    WRITE_TO_GROUND(tmgs);
    DEBUGPRINT("TMGS %d\n",sizeof(tmfi));
    return;
  }; //send_guide_stats

  void send_star_profiles(){
    static tm_star_profiles_t tmsp;
    static int curstar=0,startstar=0,i;

    static int ok=0;
    static unsigned char use[MAXSTARS],valid[MAXSTARS];

    tmsp.ssync = 0x55;
    tmsp.esync = 0xAA;
    tmsp.mid=TM_STAR_PROFILE_ID;
    tmsp.length = (unsigned short) (sizeof(tmsp)-7);
    ok=0;
    img::get_star_usevalid(&use[0],&valid[0]);
    for(i=0;i<MAXSTARS;i++){
      curstar=((startstar+i)%MAXSTARS);
      if ( (valid[curstar]) && (use[curstar])){
	img::get_profiles((unsigned char)curstar, (void *)&tmsp.spt);
	ok=1;
	break;
      }; //if valid&use 
    }; // i loop
    if (ok){
      //  printf("Profile %d\n",curstar);
      tmsp.count = GROUND_COUNTER();       
      tmsp.crc= COMPUTE_GROUND_CRC(tmsp);
      WRITE_TO_GROUND(tmsp);
      DEBUGPRINT("TMGS %d\n",sizeof(tmsp));
    };
    startstar=((curstar+1) % MAXSTARS);
  }; // send_star_profiles(){
  

}; // namespace ground

