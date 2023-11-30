#ifndef FBVIDEO_H_
#define FBVIDEO_H_

/* fbvideo.h
Module for displaying fb video while guiding. Uses the SDL libraries
for display.  

Everything in here will be within namespace fbvideo to prevent global
variable cross over.

*/
#include "string" 
// we will be using some types declared in the SDL2 library.
#include <SDL2/SDL.h>
// forward declarations of types that will be used 
#include "image_analysis.h"

//typedef struct guide_data_t guide_data_t; // frame information
//typedef struct boxes_t boxes_t; // the box structure
//typedef enum FBModes;
// frameinfo information 



// enumeration type for which display mode we are in
typedef enum display_mode_t{
  Fullchip,
  Subregions 
} display_mode_t;


//how much do we bin the display? (as opposed to the raw data)
#define DISPLAY_BINNING 2

//standard "radius" of the various shapes (in CCD pixels)
#define SHAPE_SIZE 15
#define SHAPE_CROSS_POINTS 13
#define SHAPE_CIRCLE_POINTS 13
#define SHAPE_QUAD_POINTS 5
#define SHAPE_CROSS_WIDTH 1
#define SHAPE_ROSE_POINTS 3
#define SHAPE_ROSE_TAB 5

// define some colors
#define FB_RED 0xFF0000
#define FB_GREEN 0x00BB00
#define FB_BLUE 0x0000FF
#define FB_YELLOW 0xBBBB00
#define FB_PURPLE 0xBB00BB
#define FB_CYAN 0x00FFFF

// definitions of the  options
// minimum and maximum pixel value
#define FBVIDEO_OPTIONS_MIN 1
#define FBVIDEO_OPTIONS_MAX 65535
#define FBVIDEO_OPTIONS_GAMMA 1
#define FBVIDEO_OPTIONS_BINNING DISPLAY_BINNING
#define FBVIDEO_OPTIONS_SHOWTARGET 1
#define FBVIDEO_OPTIONS_OUTLINESTARS 1
#define FBVIDEO_OPTIONS_LEGEND_POSITION 0


namespace fbvideo{

  // options
  // many are boolian, but some are not
  typedef struct options_t{

    // the folloewing are for the stretch
    // any value from 0 to device max * binning^2
    int min,max;
    // gamma for the stretch
    float gamma;
    // bitshift 
    int bitshift;

    // display mask
    unsigned char display_mask; // which boxes to display 

    // which spot to zoom in on?
    // negative means none. 
    unsigned char display_zoom;

    // target position (bool)
    char show_targets;

    // target outline stars and show their numbers
    char outline_stars;
    
    // legend position, 0 to 3 -- different corners
    char legend_position;

    // full = 0 or little = 1?
    char mode;


  }options_t;

  // initialize the video system
  // with the camera x and y size in camera pixels
  int video_start(int x, int y, int boxsize);
  // stop the video system
  int video_stop();
  // open font things
  void init_font();
  // close font things
  void destroy_font();
  // change the pallette/stretch
  // anything below min is 0, anything above max is 255, everything inbetween gets stretched
  void change_stretch(int min, int max, float gamma);
  // reset the pallette/stretch
  void reset_stretch();
  
  // display the current frame. The frame info structure carries
  // information regarding the camera mode, tracking, buffer, etc. 
  int display_frame(guide_data_t *frameinfo, boxes_t *boxes, FBModes fbm, double *xfwhm, double *yfwhm);
  
  // change the display mode
  void change_display_mode(unsigned char dm);
  char get_display_mode();
  
  // printf statement for the video screen. May be tough to do...
  // might be better off 
  int sdl_printf(int x, int y, SDL_Color color, const char *format, ...);
  
  
  // set the rendering color
  void set_color(int color);
  void set_color(int r, int g, int b);

  // various worker functions
  // quickly compute offsets and things for the shapes below.                   
  void make_shapes(int size);

  // calculate statistics
  float stats(float data[], int n, int which); 
  // draw a multi-point shape
  //  void draw_shape(float x, float y, float scale, int points, SDL_Point *spts);
  void draw_shape(float x, float y, float scale, int bin, int points, SDL_Point *spts);

  void draw_text(std::string str, float x, float y); 
  void draw_rose(float theta, float x, float y, int bin);

  // define macros for some basic shapes
#define DRAW_SCALE_CIRCLE(x,y,scale, bin) draw_shape(x, y, scale, bin, SHAPE_CIRCLE_POINTS, shape_circle_pt)
#define DRAW_CIRCLE(x,y, bin) draw_shape(x, y, 1, bin, SHAPE_CIRCLE_POINTS, shape_circle_pt)

#define DRAW_SCALE_SQUARE(x,y,scale, bin) draw_shape(x, y, scale, bin, SHAPE_QUAD_POINTS, shape_square_pt)
#define DRAW_SQUARE(x,y, bin) draw_shape(x, y, 1, bin, SHAPE_QUAD_POINTS, shape_square_pt)

#define DRAW_SCALE_DIAMOND(x,y,scale, bin) draw_shape(x, y, scale, bin, SHAPE_QUAD_POINTS, shape_diamond_pt)
#define DRAW_DIAMOND(x,y, bin) draw_shape(x, y, 1, bin, SHAPE_QUAD_POINTS, shape_diamond_pt)

#define DRAW_SCALE_CROSS(x,y,scale, bin) draw_shape(x, y, scale, bin, SHAPE_CROSS_POINTS, shape_cross_pt)
#define DRAW_CROSS(x,y, bin) draw_shape(x, y, 1, bin, SHAPE_CROSS_POINTS, shape_cross_pt)

};


#endif //FBVIDEO_H_
