/* fbvideo.cpp 
December 29, 2015.
Module for displaying FB images during flight. 

*/

#include "fbvideo.h"  // need our declarations
#include <SDL2/SDL.h> // need SDL libraries. 
#include <unistd.h>
#include <cstdio>
#include <cstdlib>

#include "fblog.h" // need to be able to access logs

#include "parameters.h" // need some parameters
#include "image_analysis.h"

// most things to do with the video will be in namespace fbvideo
namespace fbvideo{
  // video options
  options_t options;

  display_mode_t dispmode;
  // is video even on?
  unsigned char video_on=VIDEO_ON;
  
  // has video been initialzied?
  unsigned char video_open=FALSE;
  
  
  // SDL variables
  SDL_Window *main_window;
  SDL_Renderer *main_renderer;
  SDL_Surface *fontsurface;
  SDL_Texture *main_texture;
  SDL_Surface *temp_surface[3];
  SDL_Texture *temp_texture[3];

  // for displaying multiple stars in small windows
  SDL_Texture *box_texture[MAXSTARS];
  SDL_Rect box_rectangle[MAXSTARS];
  int box_center_x[MAXSTARS], box_center_y[MAXSTARS];

  // which boxes get displayed
  char box_display[MAXSTARS];
  // which spot gets zoomed in on
  char box_zoom;

  // video pallette for grayscale
  unsigned char gray_pallette[FBVIDEO_OPTIONS_MAX*DISPLAY_BINNING*DISPLAY_BINNING];
  unsigned char gray_pallette_unbinned[FBVIDEO_OPTIONS_MAX];

  // window width and height
  // note that this will be related to the chip size via DISPLAY_BINNING
  int video_width, video_height, video_box_size;
  // image width and height, and sub-box size.
  int img_width, img_height, img_box_size;

  //pixel shift
  // the pixel shift executed when converting to gray scale
  int video_pix_shift=8;

  // for shapes
  SDL_Point shape_cross_pt[SHAPE_CROSS_POINTS];
  SDL_Point shape_circle_pt[SHAPE_CIRCLE_POINTS];
  SDL_Point shape_square_pt[SHAPE_QUAD_POINTS];
  SDL_Point shape_diamond_pt[SHAPE_QUAD_POINTS];

 

  // print to the display. This is trickier than it looks. 
  int sdl_printf(int x, int y, SDL_Color color, const char *format, ...){

    return 0;
  };

  // start the video, with the camera x and y size (in camera pixels)
  int video_start(int width, int height, int boxsize){
    video_open = FALSE;
    // we only do something if the video is to be initialized
    if(video_on){
      // initialzie the video system
      if(SDL_Init(SDL_INIT_VIDEO) < 0){
	fblog::logerr("Video could not be initialized: %s",SDL_GetError());
	video_on = FALSE;
	video_open = FALSE;
	return -1;
      };

      // define the window size
      img_width = width;
      img_height = height;
      img_box_size = boxsize;

      // the video is squished additionally
      video_width = width/DISPLAY_BINNING;
      video_height = height/DISPLAY_BINNING;
      // the box size is not affected by binning
      img_box_size = boxsize;

      // create the main window
      main_window = SDL_CreateWindow("FIREBALL Guider",
				     100, //SDL_WINDOWPOS_UNDEFINED,
				     10, //SDL_WINDOWPOS_UNDEFINED,
				     video_width, video_height,
				     FB_WINDOW_MODE);
      // did anything go wrong? 
      if(main_window == NULL){
	fblog::logerr("Window could not be initialized: %s",SDL_GetError());
	fblog::logmsg("Turning off Video.");
	video_on = FALSE;
	video_open = FALSE;
	SDL_Quit();
	return -1;
      }; // main_window == NULL

      // create the renderer
      main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_TARGETTEXTURE);
      // did anything go wrong? 
      if(main_renderer == NULL) {
	video_on = FALSE;
	video_open = FALSE;
	fblog::logerr("Renderer could not be initialized: %s",SDL_GetError());
	fblog::logmsg("Turning off Video.");
	SDL_DestroyWindow(main_window);
	SDL_Quit();
	return -1;
      }; // main_renderer == NULL

      // create texture
      main_texture = SDL_CreateTexture(main_renderer, SDL_PIXELFORMAT_RGB888,
				       SDL_TEXTUREACCESS_STREAMING,
				       video_width, video_height);

      if (main_texture == NULL){
	video_on = FALSE;
	video_open = FALSE;
	fblog::logerr("Texture could not be initialized: %s",SDL_GetError());
	fblog::logmsg("Turning off Video.");
	SDL_DestroyRenderer(main_renderer);
	SDL_DestroyWindow(main_window);
	SDL_Quit();
	return -1;
      };
      
      // associate the renderer and texture
      SDL_SetRenderTarget(main_renderer, main_texture);
      
      // wipe the window for now with orangish :) 
      SDL_SetRenderDrawColor(main_renderer,0x99,0x99,0,0);
      SDL_RenderClear(main_renderer);
      
      //create textures for the various boxes
      //make rectangles for various possible stars
      for(int idx=0; idx<MAXSTARS; idx++){
	box_texture[idx] = SDL_CreateTexture(main_renderer,
					     SDL_PIXELFORMAT_RGB888, 
					     SDL_TEXTUREACCESS_STREAMING, 
					     boxsize, boxsize);
	box_rectangle[idx].w = boxsize;
	box_rectangle[idx].h = boxsize;
	box_rectangle[idx].x = 10+(10+boxsize)*(idx%4);
	box_rectangle[idx].y = 10+(10+boxsize)*(idx/4);
	box_center_x[idx] = box_rectangle[idx].x+boxsize/2;
	box_center_y[idx] = box_rectangle[idx].y+boxsize/2;
	box_display[idx] = 1;	
      }; // for idx=0...

      // set the options
      options.display_zoom = -1;
      options.gamma = 1.0;
      if (DISPLAY_BINNING == 2) {
	options.bitshift = 10;
      } else {
	options.bitshift = 8;
      };// DISPLAY_BINNING
      options.min = 0;
      options.max=65535;
      options.show_targets = TRUE;
      options.outline_stars = TRUE;
      options.legend_position = 1;
      options.mode = 0;
      
      // make the shapes
      make_shapes(SHAPE_SIZE);

      // initialize the stretch
      change_stretch(300,2000,0.5);//options.gamma);
      //      reset_stretch();
      // when it is time to make fonts, fonts will be made here
      init_font();
      

      video_open = TRUE;
      fblog::logmsg("Video started.");
    } else {
      fblog::logmsg("Video is disabled.");
    }; // video_on
    return 0;
  };

  int video_stop(){
    if(video_open) {
      // destroy open objects and such
      for(int idx=0; idx<MAXSTARS; idx++){
	SDL_DestroyTexture(box_texture[idx]);
      }; // idx(textures)
      destroy_font();
      SDL_DestroyTexture(main_texture);
      SDL_DestroyRenderer(main_renderer);
      SDL_DestroyWindow(main_window);
      SDL_Quit();
      fblog::logmsg("Video closed.");
    } else {
      fblog::logmsg("No video to stop.");
    }; // video not open
    return 0;
  };


  // quickly compute offsets and things for the shapes below.
  void make_shapes(int size){
    int idx;
    // make circle
    double circle_angle=2*(M_PI/(SHAPE_CIRCLE_POINTS-1));
    for(idx=0;idx<SHAPE_CIRCLE_POINTS-1;idx++){
      shape_circle_pt[idx].x = floor(cos(idx*circle_angle)*size+0.5);
      shape_circle_pt[idx].y = floor(sin(idx*circle_angle)*size+0.5);
    };
    shape_circle_pt[SHAPE_CIRCLE_POINTS-1].x = shape_circle_pt[0].x;
    shape_circle_pt[SHAPE_CIRCLE_POINTS-1].y = shape_circle_pt[0].y;
    // make cross
    shape_cross_pt[0].x = SHAPE_SIZE;
    shape_cross_pt[0].y = SHAPE_CROSS_WIDTH;

    shape_cross_pt[1].x = SHAPE_CROSS_WIDTH;
    shape_cross_pt[1].y = SHAPE_CROSS_WIDTH;

    shape_cross_pt[2].x = SHAPE_CROSS_WIDTH;
    shape_cross_pt[2].y = SHAPE_SIZE;

    shape_cross_pt[3].x = -SHAPE_CROSS_WIDTH;
    shape_cross_pt[3].y = SHAPE_SIZE;

    shape_cross_pt[4].x = -SHAPE_CROSS_WIDTH;
    shape_cross_pt[4].y = SHAPE_CROSS_WIDTH;

    shape_cross_pt[5].x = -SHAPE_SIZE;
    shape_cross_pt[5].y = SHAPE_CROSS_WIDTH;

    shape_cross_pt[6].x = -SHAPE_SIZE;
    shape_cross_pt[6].y = -SHAPE_CROSS_WIDTH;

    shape_cross_pt[7].x = -SHAPE_CROSS_WIDTH;
    shape_cross_pt[7].y = -SHAPE_CROSS_WIDTH;

    shape_cross_pt[8].x = -SHAPE_CROSS_WIDTH;
    shape_cross_pt[8].y = -SHAPE_SIZE;

    shape_cross_pt[9].x = SHAPE_CROSS_WIDTH;
    shape_cross_pt[9].y = -SHAPE_SIZE;

    shape_cross_pt[10].x = SHAPE_CROSS_WIDTH;
    shape_cross_pt[10].y = -SHAPE_CROSS_WIDTH;

    shape_cross_pt[11].x = SHAPE_SIZE;
    shape_cross_pt[11].y = -SHAPE_CROSS_WIDTH;

    shape_cross_pt[12].x = SHAPE_SIZE;
    shape_cross_pt[12].y = SHAPE_CROSS_WIDTH;


    // diamond
    shape_diamond_pt[0].x = SHAPE_SIZE/DISPLAY_BINNING;
    shape_diamond_pt[0].y = 0;

    shape_diamond_pt[1].x = 0;
    shape_diamond_pt[1].y = SHAPE_SIZE/DISPLAY_BINNING;

    shape_diamond_pt[2].x = -SHAPE_SIZE/DISPLAY_BINNING;
    shape_diamond_pt[2].y = 0;

    shape_diamond_pt[3].x = 0;
    shape_diamond_pt[3].y = -SHAPE_SIZE/DISPLAY_BINNING;

    shape_diamond_pt[4].x = SHAPE_SIZE/DISPLAY_BINNING;
    shape_diamond_pt[4].y = 0;
    
    // square
    int sz = floor(SHAPE_SIZE/DISPLAY_BINNING/1.42);
    shape_square_pt[0].x = sz;
    shape_square_pt[0].y = sz;
      
    shape_square_pt[1].x = -sz;
    shape_square_pt[1].y = sz;

    shape_square_pt[2].x = -sz;
    shape_square_pt[2].y = -sz;

    shape_square_pt[3].x = sz;
    shape_square_pt[3].y = -sz;

    shape_square_pt[4].x = sz;
    shape_square_pt[4].y = sz;    
  };


  void change_stretch(int min, int max, float gamma){
    int vmin,vmax;
    float value;
    int idx;
    // do some limit checking
    if(min >= max) return;
    vmin = min;
    vmax = max;
    if(min<0)vmin = 0;
    if(max<1) vmax =1;
    // first the unbinned pallette
    if(min>FBVIDEO_OPTIONS_MAX-1)vmin = FBVIDEO_OPTIONS_MAX-1;
    if(max>FBVIDEO_OPTIONS_MAX) vmax = FBVIDEO_OPTIONS_MAX;
    for(idx = 0; idx<vmin; idx++) gray_pallette_unbinned[idx] = 0;
    for(idx = vmax; idx<FBVIDEO_OPTIONS_MAX; idx++)
      gray_pallette_unbinned[idx]=255;
    for(idx = vmin; idx<vmax;idx++){
      value = 255*pow(((idx-vmin*1.0)/(vmax-vmin)),gamma);
      gray_pallette_unbinned[idx] = floor(value);
    }; // for idx

    // next the binned pallette
    vmin = vmin*DISPLAY_BINNING*DISPLAY_BINNING;
    vmax = vmax*DISPLAY_BINNING*DISPLAY_BINNING;
    for(idx = 0; idx<vmin; idx++) gray_pallette[idx] = 0;
    for(idx = vmax; idx<FBVIDEO_OPTIONS_MAX; idx++)
      gray_pallette[idx]=255;
    for(idx = vmin; idx<vmax;idx++){
      value = 255*pow(((idx-vmin*1.0)/(vmax-vmin)),gamma);
      gray_pallette[idx] = floor(value);
    }; // for idx
    
    return;
  };// change_stretch

  void reset_stretch(){
    change_stretch(FBVIDEO_OPTIONS_MIN,
		   FBVIDEO_OPTIONS_MAX*DISPLAY_BINNING*DISPLAY_BINNING,
		   FBVIDEO_OPTIONS_GAMMA);
  }; // reset_stretch;

  void set_color(int color){
    static char r,g,b;
    if (video_open){
      r = ((color >> 16) & 0xFF);
      g = ((color >> 8) & 0xFF);
      b = color & 0xFF;
      SDL_SetRenderDrawColor(main_renderer, r, g, b, 0xFF);
    }; // video_open
  }; // set_color

  void set_color(int r, int g, int b){
    if(video_open)
      SDL_SetRenderDrawColor(main_renderer, r&0xFF, g&0xFF, b&0xFF, 0xFF);
  }; // set_color


  //  void draw_shape(float x, float y, float scale, int points, SDL_Point *spts){
  void draw_shape(float x, float y, float scale, int _bin, int points, SDL_Point *spts){
    static SDL_Point pts[50];
    static int idx;
    static int bin;
    bin = _bin;
    if (bin<1) bin = 1;
    if (bin>2) bin = 2;
    if(video_open){
      for (idx =0;idx<points;idx++){
	pts[idx].x = floor(spts[idx].x*scale+x/bin+0.5);
	pts[idx].y = floor(spts[idx].y*scale+y/bin+0.5);
      };
      SDL_RenderDrawLines(main_renderer, pts, points);
    }; //video_open
    return; 
  };// draw_shape
  

  void draw_rose(float theta, float x, float y, int bin){
    static SDL_Point pts[SHAPE_ROSE_POINTS];
    static float crot, srot;
    crot = cos(theta); srot = sin(theta);
    pts[0].x = SHAPE_SIZE*(DIRECTION_CE)*crot;
    pts[0].y = SHAPE_SIZE*(DIRECTION_CE)*srot;
    pts[1].x = 0;
    pts[1].y = 0;
    pts[2].x = -2*SHAPE_SIZE*(DIRECTION_EL)*srot;
    pts[2].y = 2*SHAPE_SIZE*(DIRECTION_EL)*crot;
    
    draw_shape(x,y,1.0, bin, SHAPE_ROSE_POINTS, pts);
    
    return;
  };

  
  void copy_buffer(unsigned short int *srcbuff, SDL_Texture *desttexture, int width, int height, int _bin){
    static unsigned char pixval;
    static int bin;
    static int pitch;
    static int row,col,idx,tidx, ipixval;
    static unsigned char *pixels;
    static int max;
    bin = _bin;
    if (bin < 1) bin = 1;
    if (bin > 2) bin = 2;
    max = 0;
    if (video_open){
    // we lock the texture
      SDL_LockTexture(desttexture, NULL, (void **) &pixels, &pitch);

    for (row=0;row<height;row+=bin)
      for(col=0;col<width;col+=bin){
	// index in the source
	idx = col+row*width;

	ipixval = srcbuff[idx];
	if (ipixval> max) max = ipixval;
	if(bin==2){
	  ipixval += srcbuff[idx+1];
	  ipixval += srcbuff[idx+width];
	  ipixval += srcbuff[idx+width+1];
	  pixval = gray_pallette[ipixval];
	} else {
	  pixval = gray_pallette_unbinned[ipixval];
	}; //bin == 2
	// target indx in the texture
	tidx = (col/bin+(row/bin)*width/bin)*sizeof(uint32_t);
	*(pixels+tidx)=pixval;
	*(pixels+tidx+1)=pixval;
	*(pixels+tidx+2)=pixval;
	*(pixels+tidx+3)=pixval;
      };//col loop

    }; // video_open
    return;
  };

  void init_font(){
    return; 
  };

  void destroy_font(){
    return;
  };

  int display_frame(guide_data_t *frameinfo, boxes_t *boxes, FBModes fbm){
    static int idx;
    static FBModes fb=Idle;
    
    if (fb != fbm) {
      set_color(0x000000);
      SDL_RenderClear(main_renderer);
      fb = fbm;
    };
    //    if(fbm != MultiStarFollow && fbm != MultiStarFixed){
    if(options.mode == 0){ // full device
      //DEBUGLINE;
      //This is the full chip display. 
      copy_buffer(frameinfo->buf, main_texture, 1280, 1080, DISPLAY_BINNING);
      SDL_RenderCopy(main_renderer, main_texture, NULL, NULL);
      //      set_color(0xFF00FF);
      // 

      set_color(0xAA00AA); // magenta
      for (idx=0;idx<MAXSTARS; idx++){
	if(frameinfo->nextvalid[idx] && frameinfo->nextuse[idx])
	  DRAW_SCALE_SQUARE(frameinfo->nextx[idx], frameinfo->nexty[idx],1.5,DISPLAY_BINNING);
	if(frameinfo->nextuse[idx])
	  DRAW_DIAMOND(frameinfo->nexttx[idx], frameinfo->nextty[idx], DISPLAY_BINNING);
      }; // idx


      set_color(0x00AA00); // green
      for (idx=0;idx<MAXSTARS; idx++){
	// circle detected stars
	if(frameinfo->valid[idx] && frameinfo->use[idx])
	  DRAW_SCALE_CIRCLE(frameinfo->x[idx],frameinfo->y[idx],0.7,DISPLAY_BINNING);
	// diamonds at target positions
	if(frameinfo->use[idx])
	  DRAW_DIAMOND(frameinfo->tx[idx], frameinfo->ty[idx],DISPLAY_BINNING);
      }; // idx (Circles)

      
      // field center and rotation center
      set_color(0xFFFFFF);
      DRAW_CROSS(frameinfo->field_cx, frameinfo->field_cy,DISPLAY_BINNING);
      set_color(0xAA6600);
      DRAW_CROSS(frameinfo->rx, frameinfo->ry,DISPLAY_BINNING);
      set_color(0xAAAA00);
      draw_rose(frameinfo->field_rot,ROSE_X,ROSE_Y,DISPLAY_BINNING);

    } else {  // small window
      //      DEBUGLINE;
      for(idx=0;idx<MAXSTARS;idx++){
	if(frameinfo->valid[idx] && frameinfo->use[idx]){
	copy_buffer(boxes->boxes[idx], box_texture[idx],boxes->size,boxes->size,1);
	set_color(0xFF0000);
	//	DEBUGLINE;
	SDL_RenderCopy(main_renderer, box_texture[idx],NULL,&box_rectangle[idx]);
	if(fbm == MultiStarFollow){
	DRAW_CROSS(frameinfo->tx[idx]-frameinfo->x[idx]+box_center_x[idx],
		   frameinfo->ty[idx]-frameinfo->y[idx]+box_center_y[idx],
		   1);
	} else {
	DRAW_CROSS(box_center_x[idx],
		   box_center_y[idx],
		   1);
	};
	set_color(0xAAAA00);
	draw_rose(frameinfo->field_rot,ROSE_X,ROSE_Y,DISPLAY_BINNING);
	//	DEBUGPRINT("%s\n",SDL_GetError());
	set_color(0xAAAA00);
	SDL_RenderDrawRect(main_renderer,&box_rectangle[idx]);
	};// valid
	
      }; // idx loop
    }; // which mode?
    
    SDL_RenderPresent(main_renderer);
    return 0;
  };


  void change_display_mode(unsigned char dm){
    if(dm==1) { 
      options.mode = 1; 
    }  else { 
      options.mode = 0;
    };
    return;
  }; // change_display_mode


  char get_display_mode(){
    return options.mode;
  };// get_display_mode

}; // namespace fbvideo


