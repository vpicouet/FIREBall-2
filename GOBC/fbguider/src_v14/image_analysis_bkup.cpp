#include <pthread.h>
#include <math.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstdint>

#include "image_analysis.h"
#include "fblog.h"
#include "fbvideo.h"
#include "parameters.h"
#include "fbcamera.h"
#include "timing.h"
#include "gondola_communication.h"
#include "ground_communication.h"
#include "adio.h"

// for frameblobs.
#include "frameblob.h"
#include "bloblist.h"

namespace img{
  // variables
  
  FBModes fbmode;
  boxes_t boxes;
  guide_data_t guidedata;
  attitude_t attitude;

  bloblist *pBlobs[IMAGE_BUFFERS];
  frameblob *fblob;
  

  pthread_t imageworker_thread[IMAGE_BUFFERS];
  
  pthread_mutex_t imageworker_mutex=PTHREAD_MUTEX_INITIALIZER;
  

  // functions


  void process_frame(camera_parameters_t *campar, int bufidx){
    // fill out the guide parameters here...
    // fire off the worker thread!
    guidedata.buf = (*campar).picbuf[bufidx];
    guidedata.bufidx = bufidx;
    guidedata.framenumber = (*campar).framenumber;
    pthread_create(&imageworker_thread[bufidx], NULL, imageworker, NULL);
    pthread_detach(imageworker_thread[bufidx]);
  };

  // initialize the guide data structure.
  void imginit(int w, int h,int boxsize){
    // initialize boxes
    init_boxes(boxsize);

    // initialize guide data
    guidedata.snrcut = IMG_SNR;
    guidedata.width = w;
    guidedata.height = h;

    guidedata.rx = ROTATION_CENTER_X;
    guidedata.ry = ROTATION_CENTER_Y;

    memset(guidedata.tx,0,MAXSTARS*sizeof(float));
    memset(guidedata.ty,0,MAXSTARS*sizeof(float));
    memset(guidedata.tf,0,MAXSTARS*sizeof(float));
    memset(guidedata.tw,0,MAXSTARS*sizeof(float));

    memset(guidedata.x,0,MAXSTARS*sizeof(float));
    memset(guidedata.y,0,MAXSTARS*sizeof(float));
    memset(guidedata.f,0,MAXSTARS*sizeof(float));

    memset(guidedata.lastx,0,MAXSTARS*sizeof(float));
    memset(guidedata.lasty,0,MAXSTARS*sizeof(float));
    memset(guidedata.lastf,0,MAXSTARS*sizeof(float));

    memset(guidedata.use,0,MAXSTARS);
    memset(guidedata.valid,0,MAXSTARS);
    memset(guidedata.lastvalid,0,MAXSTARS);





    // initialize frameblobs

    guidedata.fb_bits = FB_BITS;
    guidedata.fb_cam_sat=FB_CAM_SAT;
    guidedata.fb_plate_scale=FB_PLATE_SCALE;
    guidedata.fb_coarse_grid=FB_COARSE_GRID;
    guidedata.fb_cen_box=FB_CEN_BOX;
    guidedata.fb_ap_box=FB_AP_BOX;
    guidedata.fb_sigma_thresh=FB_SIGMA_THRESH;
    guidedata.fb_dist_tol=FB_DIST_TOL;

    guidedata.fbmode = Idle;

    guidedata.initialized = 1;

    fblob = new frameblob[3];
    for(int idx = 0; idx<IMAGE_BUFFERS; idx++){
      fblob[idx]=frameblob(guidedata.buf,
			   guidedata.width,guidedata.height,
			   guidedata.fb_bits,
			   guidedata.fb_plate_scale);

      fblob[idx].set_satval(guidedata.fb_cam_sat);
      fblob[idx].set_grid(guidedata.fb_coarse_grid);
      fblob[idx].set_cenbox(guidedata.fb_cen_box);
      fblob[idx].set_apbox(guidedata.fb_ap_box);
      fblob[idx].set_threshold(guidedata.fb_sigma_thresh);
      fblob[idx].set_disttol(guidedata.fb_dist_tol*guidedata.fb_dist_tol);
      
    };// for idx

      
			   
    return;
  }; //imginit();

  void imgclose(){
    free_boxes();
    guidedata.initialized = 0;
    return;
  }; //imgclose();

  
  void init_boxes(int halfsize){
    int sz;
    sz = halfsize;
    if (sz < 10) sz = 10;
    if (sz > 80) sz = 80;
    boxes.halfwidth = sz;
    boxes.halfheight = sz;
    boxes.size = 2*sz+1;
    boxes.pitch = boxes.size*sizeof(uint16_t);
    boxes.star_cut = BOX_STAR_CUT;
    for(int idx=0;idx<MAXSTARS;idx++){
      boxes.boxes[idx] = (uint16_t *)malloc((2*sz+1)*(2*sz+1)*sizeof(uint16_t));
    }; // for idx
    boxes_compute_star_cut(boxes.star_cut);
    boxes.initialized=1;
    return;
  };// init_boxes();

  void boxes_compute_star_cut(int scut){
    int size,fullsize,starcut;
    int row, column,idx, tgpoint, bgpoint, arrpoint;
    int *xvals, *yvals, *tvals, *poff;
    float *dist, xsqsum, ysqsum;
    starcut = scut;
    size = 2*starcut+1;
    fullsize = 4*starcut+1;
    
    if (starcut < 1) {
      starcut = 1;
      size = 3;
      fullsize = 5;
    };// size < 3; 
    if (starcut > MAX_STAR_CUT) {
      starcut = MAX_STAR_CUT;
      size = 2*MAX_STAR_CUT+1;
      fullsize = 4*MAX_STAR_CUT+1;
    }; // starcut > MAX_STAR_CUT;
    fblog::logmsg("Making boxes with size %d",size);
    
    xvals = (int*) malloc(fullsize*fullsize*sizeof(int));
    yvals = (int*) malloc(fullsize*fullsize*sizeof(int));
    tvals = (int*) malloc(fullsize*fullsize*sizeof(int));
    dist = (float*) malloc(fullsize*fullsize*sizeof(float));
    poff = (int*) malloc(fullsize*fullsize*sizeof(int));

    tgpoint = 0;
    bgpoint = 0;
    arrpoint = 0;
    xsqsum = 0;
    ysqsum = 0;
    for(row=0;row<fullsize;row++){
      for(column=0;column<fullsize;column++){
        idx = column+row*fullsize;
	xvals[idx] = column-2*starcut;
	yvals[idx] = row-2*starcut;
	tvals[idx] = 0;
	dist[idx] = sqrt(1.0*xvals[idx]*xvals[idx]+yvals[idx]*yvals[idx]);
	poff[idx] = (column-2*starcut)+(row-2*starcut)*boxes.size;
	if (dist[idx] <= 2*starcut && dist[idx] > starcut) {
	  tvals[arrpoint] = -1;
	  xvals[arrpoint] = xvals[idx];
	  yvals[arrpoint] = yvals[idx];
	  poff[arrpoint] = poff[idx];
	  xsqsum += xvals[idx]*xvals[idx];
	  ysqsum += yvals[idx]*yvals[idx];
	  bgpoint++;
	  arrpoint++;
	}; 
	if (dist[idx] <= starcut){
	  tvals[arrpoint] = 1;
	  xvals[arrpoint] = xvals[idx];
	  yvals[arrpoint] = yvals[idx];
	  poff[arrpoint] = poff[idx];
	  tgpoint++;
	  arrpoint++;
	};
      };//column
    };//row
    // at this point we have arrpoint values that are either bg points
    // or target points, so we're going to fill out the appropriate
    // values out in boxes.  

    if(arrpoint>0){
      boxes.xmult = (int *) malloc(arrpoint*sizeof(int));
      boxes.ymult = (int *) malloc(arrpoint*sizeof(int));
      boxes.type = (int *) malloc(arrpoint*sizeof(int));
      boxes.pixoff = (int *) malloc(arrpoint*sizeof(int));
      boxes.star_cut = starcut;
      boxes.star_cut_star_count = tgpoint;
      boxes.star_cut_bg_count = bgpoint;
      boxes.star_cut_count = arrpoint;
      // transfer the arrays
      memcpy(boxes.xmult, xvals, arrpoint*sizeof(int));
      memcpy(boxes.ymult, yvals, arrpoint*sizeof(int));
      memcpy(boxes.type, tvals, arrpoint*sizeof(int));
      memcpy(boxes.pixoff, poff, arrpoint*sizeof(int));
      //      for(int jdx=0;jdx<arrpoint;jdx++)
      //	printf("%d %d %d %d\n",boxes.xmult[jdx], boxes.ymult[jdx], boxes.type[jdx], boxes.pixoff[jdx]);
    } else {
      fblog::logerr("Not enough starpoints!");
    };//arrpoint>0


    // free memory
    free(xvals);
    free(yvals);
    free(tvals);
    free(dist);
    free(poff);
    return;
  }; //boxes_compute_star_cut();

  void boxes_destroy_star_cut(){
    if(boxes.star_cut_count > 0){
      free(boxes.pixoff);
      free(boxes.type);
      free(boxes.ymult);
      free(boxes.xmult);
      //      DEBUGLINE;
    }; // boxes exist

    boxes.star_cut_count = 0;
    boxes.star_cut_star_count = 0;
    boxes.star_cut_bg_count=0;
    

    // just free the memory
    
  }; //boxes_destroy_star_cut();

  void boxes_redo_star_cut(int starcut){
    boxes_destroy_star_cut();
    boxes_compute_star_cut(starcut);
  };//boxes_redo_star_cut();

  void free_boxes(){
    if(boxes.initialized){
      boxes_destroy_star_cut();
      for(int idx=MAXSTARS-1; idx>=0; idx--)
	free(boxes.boxes[idx]);
    }; // boxes.initialized
  }; // free_boxes();

  void extract_boxes(uint16_t *buf, float *cx, float *cy, unsigned char *success, unsigned char *use){
    static int idx, jdx,x,y;
    static uint16_t *ptrs, *ptrt;
    memset(success,0,MAXSTARS);
      if(boxes.initialized && guidedata.initialized){
	// loop over boxes
	for(idx=0;idx<MAXSTARS;idx++){
	  // only extract the ones we are asked to...
	  if(use[idx]) {
	    x = floor(cx[idx]);
	    y = floor(cy[idx]);
	    
	  // process only if the point is within the core of the image
	    if ( x>boxes.halfwidth && x<guidedata.width-boxes.halfwidth-1 && y>boxes.halfheight && y<guidedata.height-boxes.halfheight-1){
	      ptrs = buf+(x-boxes.halfwidth+(y-boxes.halfheight)*guidedata.width);
	      ptrt = boxes.boxes[idx];
	      // copy memory
	      for(jdx=0;jdx<boxes.size;jdx++){
		memcpy(ptrt,ptrs,boxes.pitch);
		ptrt += boxes.size;
		ptrs += guidedata.width;
	      }; // jdx
	      boxes.cx[idx]=x;
	      boxes.cy[idx]=y;
	      success[idx] = TRUE;
	    }; // within bounds
	  }; // use?
	}; // for(idx)
      }; // boxes&&guidedata initialized
    return;
  }; // extract_boxes();


  void centroid_boxes(float *cx, float *cy, float *cf, float *sigx, float *sigy, unsigned char *valid, unsigned char *use){
    //    static int count=0;
    static int idx, jdx, row;
    static int max, rowmax, colmax, maxidx;
    static uint16_t *ptr;
    static int bigloop;
    static int cury, curx, fgct, bgct;
    static float curpix;
    static float fgsum, bgsum, fgxsum, fgysum, fgxsqsum, fgysqsum;
    static float sig_x, sig_y, c_x, c_y, noise;
    static float bgsqsum, snr,  bg_level;
    unsigned char curval;
    //    DEBUGPRINT("centroid: %d\n",count++);
    max = rowmax = colmax = -1;
    if(boxes.initialized && guidedata.initialized){
      for(idx=0;idx<MAXSTARS;idx++){
	// only compute centroid if we want to use this box.
	curval=valid[idx];
	valid[idx]=0;
	if(use[idx] && curval){
	  // we start with the assumption that the computation will fail
	  max=0;
	  // first, find the max row/column
	  ptr=boxes.boxes[idx];
	  bigloop = boxes.size*boxes.size;
	  for(row=0;row<bigloop;row++){
	    if(*ptr > max){
	      max = *ptr;
	      maxidx = row;
	    };// new max!
	    ptr++;
	  }; // row (but really entire box);-+
	  rowmax = maxidx/boxes.size;
	  colmax = maxidx%boxes.size;

	  //	  DEBUGPRINT("max: %d %d %d\n",rowmax,colmax,max);

	  // only compute the centroid if far sufficiently far away
	  // from the edge of the box 
	  if(rowmax > boxes.star_cut && colmax > boxes.star_cut &&
	     rowmax < boxes.size - boxes.star_cut -1 &&
	     colmax < boxes.size - boxes.star_cut -1){
	    fgsum = 0;
	    bgsum = 0;
	    fgxsum = 0;
	    fgysum = 0;
	    fgxsqsum = 0;
	    fgysqsum = 0;
	    bgsqsum = 0;
	    fgct = 0;
	    bgct = 0;
	    c_x = 0;
	    c_y = 0;
	    sig_x = 0;
	    sig_y = 0;
	    for(jdx=0;jdx<boxes.star_cut_count; jdx++){
	      curx = colmax+boxes.xmult[jdx];
	      cury = rowmax+boxes.ymult[jdx];
	      //DEBUGPRINT("curx=%d cury=%d\n",curx,cury);
	      // make sure the pixel being inspected is within range
	      if(curx>=0 && curx <boxes.size &&
		 cury>=0 && cury <boxes.size){
		// what is the pixel?
		//		curpix = boxes.boxes[guidedata.bufidx][curx+cury*boxes.size];
		curpix = boxes.boxes[idx][curx+cury*boxes.size];
		if(boxes.type[jdx] == 1){
		  fgct++;
		  fgsum += curpix;
		  fgxsum += curpix*boxes.xmult[jdx];
		  fgysum += curpix*boxes.ymult[jdx];
		  fgxsqsum += curpix*boxes.xmult[jdx]*boxes.xmult[jdx];
		  fgysqsum += curpix*boxes.ymult[jdx]*boxes.ymult[jdx];	  
		} else {
		  bgct++;
		  bgsum += curpix;
		  bgsqsum += (1.0*curpix)*curpix;
		}; // which type box is it?		
		}; // curx, cury in range
	    }; //jdx star_cut_ct
	    bg_level = bgsum/bgct;
	    noise = sqrt(bgsqsum/bgct - bg_level*bg_level);
	    if (noise < 1) noise = 1;
	    fgsum = fgsum - fgct*bg_level;
	    snr = fgsum/noise;
	    fgxsqsum = fgxsqsum - boxes.xsqsum*bg_level;
	    fgysqsum = fgysqsum - boxes.ysqsum*bg_level;
	    // fgx/ysum already takes the bg into account (odd in x,y)
	    c_x = fgxsum/fgsum;
	    c_y = fgysum/fgsum;
	    sig_x = sqrt(fgxsqsum/fgsum-c_x*c_x);
	    sig_y = sqrt(fgysqsum/fgsum-c_y*c_y);
	    // OK. We've computed all sorts of things...
	    valid[idx]=0;
	    //	    DEBUGLINE;
	    if (snr > IMG_SNR && sig_x < STAR_SIG_MAX && sig_y < STAR_SIG_MAX && sig_x > STAR_SIG_MIN && sig_y> STAR_SIG_MIN ){
	      c_x = c_x+colmax-boxes.size/2.0+boxes.cx[idx]+1;
	      c_y = c_y+rowmax-boxes.size/2.0+boxes.cy[idx]+1;
	      if ( c_x > boxes.halfwidth && c_y > boxes.halfwidth && c_x < guidedata.width-boxes.halfwidth-1 && c_y < guidedata.height-boxes.halfwidth-1 ){
		valid[idx] = 1;
		cf[idx] = fgsum;
		cx[idx] = c_x;
		cy[idx] = c_y;
		sigx[idx] = sig_x;
		sigy[idx] = sig_y;
		//printf("SNR:: %d %5.2lf %5.2f %5.2f %5.2lf %5.2lf\n",idx,snr, IMG_SNR ,fgsum, noise, bg_level);
	      }; // star is not near the edge. 
	      //	      
	    }; // good snr
	  }; // centroid in box
      };//use[idx];
      };//for(idx)
    }; // boxes & guidedata initialized
    
  };//centroid_boxes
  


  FBModes getFBMode(){
    return guidedata.fbmode;
  }; //getFBMode();

  long getFramenumber(){
    return guidedata.framenumber;
  }; // getFramenumber;

  void setFBMode(FBModes fbm){
    if(fbm >=0 && fbm < NFBMODES){
      guidedata.nextfbmode = fbm;
      guidedata.changefbmode = 1;
      };
    return;
  }; //setFBMode();
  
  
  void *imageworker(void *data){
    static guide_data_t *gd;
    static FBModes fbm;
    static unsigned char nstars;
    static float x[MAXSTARS],y[MAXSTARS],f[MAXSTARS];
    static unsigned char valid[MAXSTARS];
    static float sigx[MAXSTARS], sigy[MAXSTARS];
    static int idx, valcnt;
    static float dx, dy, drot, pa, lastpa;
    static float dEL, dCE;
    static float lastdx, lastdy, lastdrot;
    static float gel,gaz,grot;
    static char msg[255];

    pthread_mutex_lock(&imageworker_mutex);
    // first copy the structures, so that they are not changed from outside
    // 
    gd = &guidedata;
    
    // are we being asked to change the fbmode?
    if (gd->changefbmode){
      gd->fbmode = gd->nextfbmode;
      switch(gd->fbmode){
      case Idle:
	gondola::update_guider_status(0);
	break;
      case Acquire:
	gondola::update_guider_status(0);
	break;
      case MultiStarFixed:
	gondola::update_guider_status(1);
	break;
      case MultiStarFollow:
	gondola::update_guider_status(1);
	break;
      default:
	break;
      }
      gd->changefbmode = 0;
    };// changed guide mode

    fbm = gd->fbmode;

    // reset the values -- these are what will be 
    // reported, if no other values are computed.
    dx=0; dy=0; drot=0;

    fblog::logmsg("Processing frame: %ld",gd->framenumber);

    //    if (fbm == None) fbm = Acquire0;

    // some preliminary conditionals.
    // ---------------------------------
    // were we asked to switch the targets? if so, do so.
    // were we asked to switch the stars? if so, do so.
    // are we being asked to update targets? 
    // are we being asked to update stars? 
    // any other changes?!
    
    // are we being asked to change targets? 
    if(gd->nexttargets){
      fblog::logmsg("IMGWORKER: Switching to next target,\n");
      memcpy(gd->tx, gd->nexttx,MAXSTARS*sizeof(float));
      memcpy(gd->ty, gd->nextty,MAXSTARS*sizeof(float));
      memcpy(gd->valid, gd->nextvalid, MAXSTARS);
      memcpy(gd->use, gd->nextuse, MAXSTARS);
      // reset the flag
      memset(gd->nextvalid,0,MAXSTARS);
      for (idx=0;idx<MAXSTARS;idx++){
	gd->nexttx[idx]=-1.0;
	gd->nextty[idx]=-1.0;
      };
      gd->nexttargets = 0;
    }; // if(gd.nexttargets);

    // are we being asked to change stars?
    if(gd->nextstars){
      memcpy(gd->x, gd->nextx, MAXSTARS*sizeof(float));
      memcpy(gd->y, gd->nexty, MAXSTARS*sizeof(float));
      memcpy(gd->valid, gd->nextvalid, MAXSTARS);
      // reset the flag
      gd->nextstars = 0;
    };// if(gd.nextstars)

    //are we being asked to update targets? 
    if(gd->updatetargets){
      memcpy(gd->tx, gd->nexttx, MAXSTARS*sizeof(float));
      memcpy(gd->ty, gd->nextty, MAXSTARS*sizeof(float));
      memcpy(gd->valid, gd->nextvalid, MAXSTARS);
      // reset the flag
      gd->updatetargets=0;
    }; // if(gd.updatetargets);

    //are we being asked to update stars?
    if(gd->updatestars){
      // do stuff
      gd->updatestars=0;
    }; // if(gd->updatestars);
    
    // giant switch that decides what to do, depending on the mode.
    switch(fbm){
    case Idle:
      // do nothing -- just display later
      break;
    case Acquire:
      //acq_func(gd->buf, gd->x, gd->y, gd->f, gd->valid, &nstars, gd->bufidx);
      acq_func(gd->buf, x, y, f, valid, &nstars, gd->bufidx);
      // CHANGE WHICH MODE IS INVOKED AUTOMATICALLY
      if (nstars >0) { 
	guidedata.fbmode = MultiStarFollow;
      	gondola::update_guider_status(1);
	sprintf(msg,"Acquired %d stars.",nstars);
	ground::send_text(msg);
      };
      //if (nstars >0) guidedata.fbmode = MultiStarFixed;
      //copy all the values.
      DEBUGPRINT("Nstars = %d\n",nstars);
      memcpy(guidedata.x,x,MAXSTARS*sizeof(float));
      memcpy(guidedata.y,y,MAXSTARS*sizeof(float));
      memcpy(guidedata.tx,x,MAXSTARS*sizeof(float));
      memcpy(guidedata.ty,y,MAXSTARS*sizeof(float));
      memcpy(guidedata.f,f,MAXSTARS*sizeof(float));
      memcpy(guidedata.tf,f,MAXSTARS*sizeof(float));
      // correct this.
      memcpy(guidedata.valid,valid,MAXSTARS);
      for(idx=0;idx<STARS;idx++){
	guidedata.valid[idx] = 1;
	guidedata.use[idx] = 1;
	guidedata.tw[idx] = log(guidedata.tf[idx]);
      }; // idx
      break;
    case MultiStarFixed:
      extract_boxes(gd->buf, gd->tx,gd->ty, valid, guidedata.use);
      centroid_boxes(x,y,f, sigx, sigy, valid, guidedata.use);
      for(idx=0; idx<MAXSTARS; idx++){
	if(gd->tf[idx] < 0 && valid[idx]){
	  gd->tf[idx] = f[idx];
	  gd->tw[idx] = log(1+f[idx]);
	};
	if (valid[idx]) {
	  guidedata.x[idx]=x[idx];
	  guidedata.y[idx]=y[idx];
	  guidedata.f[idx]=f[idx];
	  guidedata.valid[idx]=valid[idx];
	} else {
	  guidedata.valid[idx]=0;
	}; // valid
      }; //idx
      //memcpy(guidedata.x,x,MAXSTARS*sizeof(float));
      //memcpy(guidedata.y,y,MAXSTARS*sizeof(float));
      //memcpy(guidedata.f,f,MAXSTARS*sizeof(float));
      //memcpy(guidedata.valid, valid, MAXSTARS); // THIS
      // add here, a change to tw if tf is negative.
      compute_mapping(guidedata.tx, guidedata.ty,
                      guidedata.x, guidedata.y,
                      guidedata.tw, guidedata.valid,
		      guidedata.use, 
                      STARS,
                      &dx, &dy, &drot);
      printf("--> %lf\t%6.2f %6.2f %6.2f %6.2f\t %6.2f %6.2f %6.2f %6.2f\t%6.2f %6.2f %6.2f %6.2f\t %6.2f %6.2f %7.3f\n",
             get_time(),
             guidedata.tx[0], guidedata.ty[0],
             guidedata.x[0], guidedata.y[0],
             guidedata.tx[1], guidedata.ty[1],
             guidedata.x[1], guidedata.y[1],
             guidedata.tx[2], guidedata.ty[2],
             guidedata.x[2], guidedata.y[2],
             dx, dy, drot);

      break;
    case MultiStarFollow:
      extract_boxes(gd->buf, gd->x,gd->y, valid, guidedata.use);
      centroid_boxes(x,y,f, sigx, sigy, valid, guidedata.use);

      for(idx=0; idx<MAXSTARS; idx++){
	if(gd->tf[idx] < 0 && valid[idx]){
	  gd->tf[idx] = f[idx];
	  gd->tw[idx] = log(1+f[idx]);
	}; //negative flux
	if (valid[idx]) {
	  guidedata.x[idx]=x[idx];
	  guidedata.y[idx]=y[idx];
	  guidedata.f[idx]=f[idx];
	  guidedata.valid[idx]=valid[idx];
	} else {
	  guidedata.valid[idx]=0;
	}; // valid
      }; //idx
      // MM 160519
      /* memcpy(guidedata.x,x,MAXSTARS*sizeof(float));
      memcpy(guidedata.y,y,MAXSTARS*sizeof(float));
      memcpy(guidedata.f,f,MAXSTARS*sizeof(float));
      memcpy(guidedata.valid, valid, MAXSTARS); */ //THIS
      // add here, a change to tw if tf is negative.
      compute_mapping(guidedata.tx, guidedata.ty,
		      guidedata.x, guidedata.y,
		      guidedata.tw, guidedata.valid,
		      guidedata.use, 
		      STARS, 
		      &dx, &dy, &drot);
      float raddrot;
      raddrot = drot/180.*3.14159;
      for(idx = 0; idx< MAXSTARS; idx++){
	if (guidedata.valid[idx]==0 && guidedata.use[idx]==1) {
       	  raddrot=0.0;  // Should this be commented out----confirm why is this in here?
	  guidedata.x[idx] = guidedata.rx + (guidedata.tx[idx]-guidedata.rx)*cos(raddrot)-(guidedata.ty[idx]-guidedata.ry)*sin(raddrot)+dx;
	  guidedata.y[idx] = guidedata.ry + (guidedata.tx[idx]-guidedata.rx)*sin(raddrot)+(guidedata.ty[idx]-guidedata.ry)*cos(raddrot)+dy;
	};
      };
             printf("--> %lf\t%6.2f %6.2f %6.2f %6.2f\t %6.2f %6.2f %6.2f %6.2f\t%6.2f %6.2f %6.2f %6.2f\t %6.2f %6.2f %7.3f\t%d, %d, %d, %d\t%d, %d, %d\n",
	     get_time(),
	     guidedata.tx[0], guidedata.ty[0], 
	     guidedata.x[0], guidedata.y[0],
	     guidedata.tx[1], guidedata.ty[1], 
	     guidedata.x[1], guidedata.y[1],
	     guidedata.tx[2], guidedata.ty[2], 
	     guidedata.x[2], guidedata.y[2],
	     dx, dy, 
	     drot,
	     guidedata.valid[0],
	     guidedata.valid[1],
	     guidedata.valid[2],
	     guidedata.valid[3],
	     guidedata.use[0],
	     guidedata.use[1],
	     guidedata.use[2],
	     guidedata.use[3]);  // added this last line
      break;
    default: // some other mode...
      break;
    }; // switch
    

    switch(fbm){
      // these are the principal guide modes
      // we have a computation of dx, dy, and drot at this point.
    case MultiStarFixed:
    case MultiStarFollow:
      valcnt = 0;
      for(idx=0;idx<MAXSTARS;idx++)
	if (valid[idx] && guidedata.use[idx]) valcnt++;

      if(valcnt>0){
	lastdx = dx;
	lastdy = dy;
	lastdrot = drot;
	lastpa = pa;
	// to compute conversion from from dx,dy to arcseconds,
	// we need to know the current position angle
	// we could get this from the gondola
	// we could get this from our astrometry solution
	// we could get this from an uploaded ground value
	// (some of these would need to be updated
	if(gondola::get_gondola_boresight(&gaz,&gel,&grot)){
	  pa = (grot)*M_PI/180.;
	  guidedata.field_rot = pa;
	}else {
	  pa=lastpa;
	};
	DEBUGPRINT("Gondola rot: %f\n",grot);
	// FOR CU LAB TESTING: remove pa=0.0 below if all is working   
	//pa = 0.0;
	compute_elce(pa, ROTATOR_OFFSET, dx, dy, &dEL, &dCE);
	// update the values
	gondola::update_guider_rotation_error(drot,1);
	// write analog voltages
	adio::analog_out_both(dCE*VOLTSBINPIX, dEL*VOLTSBINPIX);
	// send the rotation error. 
	gondola::send_guider_rotation_error();
	// compute the _current_ guider alpha/delta/pa based on the 
	// error and _guider_ understanding of position.
	
      } else { // valcnt>0
	// again, find out what the PA is here...
	if(gondola::get_gondola_boresight(&gaz,&gel,&grot)){
	  pa = (grot)*M_PI/180.;
	  guidedata.field_rot = pa;
	}else {
	  pa=lastpa;
	};
	DEBUGPRINT("Gondola rot: %f\n",grot);
	// FOR CU LAB TESTING: remove pa=0.0 below if all is working   
	// pa = 0.0;
	compute_elce(pa, ROTATOR_OFFSET, lastdx, lastdy, &dEL, &dCE);
	//gondola::update_guider_rotation_error(drot*180.00/M_PI,0);
	gondola::update_guider_rotation_error(0.0,0);
	// write analog voltages
	//adio::analog_out_both(dCE*VOLTSBINPIX, dEL*VOLTSBINPIX);
	adio::analog_out_both(0.00, 0.00);
	// send the rotation error. 
	gondola::send_guider_rotation_error();
      }; // valcnt <= 0

      break;
      // these are the not-doing-anything here modes
    case Idle:
    case Acquire:
    default:
      break;
    }; //switch fbm

    if ((gd->framenumber % 60) == 2){
      fbvideo::display_frame(gd,&boxes, fbm);
    }
    pthread_mutex_unlock(&imageworker_mutex);

    return NULL;
  };
  

  void acq_func(uint16_t *buf, float *x, float *y, float *flux, unsigned char *valid, unsigned char *nstars, int index){
    static bloblist *pb;
    static frameblob *fb;
    static int i, stars_found;

    frameblob fblob(buf, 1280, 1080, 16, 1.0);
    fb = &fblob;
    


    //    fb = fblob+index;
    //    fb->setMap(buf);
    fb->clearblobs();

    fb->set_satval(guidedata.fb_cam_sat);
    fb->set_grid(guidedata.fb_coarse_grid);
    fb->set_cenbox(guidedata.fb_cen_box);
    fb->set_apbox(guidedata.fb_ap_box);
    fb->set_threshold(guidedata.fb_sigma_thresh);
    fb->set_disttol(guidedata.fb_dist_tol*guidedata.fb_dist_tol);

    fb->calc_mapstat();
    fb->calc_searchgrid();
    fb->calc_centroid();
    fb->calc_flux();
    fb->fix_multiple();
    fb->sortblobs();
    stars_found = fb->Numblobs();
    
    // zero out the valid flags.
    memset(valid,0,MAXSTARS);

    pb = fb->getblobs();
    *nstars = std::min(MAXSTARS, stars_found);
    //    DEBUGPRINT("stars %d\n",*nstars);
    
    for(i=0; i<*nstars; i++){
      x[i] = pb->getx();
      y[i] = pb->gety();
      flux[i] = pb->getflux();
      //      DEBUGPRINT("%d %f %f %f\n",i,x[i],y[i],flux[i]);
      valid[i] = 1;
      pb = pb->getnextblob();
    };
  }; // acq_


  int compute_mapping(float *x0, float *y0, 
		      float *x1, float *y1, 
		      float *weight, unsigned char *valid,
		      unsigned char *use,
		      char points, 
		      float *dx, float *dy, float *drot){

    static int i; // counter                                                   
    // centroids                                                               
    static float cx,cy, count ,cx1, cy1;
    static float sumx, sumy, sumweight, sumrot, crossp;
    static float ux,uy,wx, wy, tmp, ang;
    cx =0; cy=0; count=0; cx1=0; cy1=0;
    sumx =0; sumy=0; sumweight=0;
    ux = 0; uy =0; wx=0; wy=0;
    sumrot = 0; crossp = 0;
    ang = 0;

    // if there is only 1 point or no points, return the obvious.
    if (points == 0){
      (*dx)=0;
      (*dy)=0;
      (*drot)=0;
      return 0;
    };

    if (points == 1){
      (*dx) = x1[0]-x0[0];
      (*dy) = y1[0]-y0[0];
      (*drot) = 0;
      return 0;
    };

    // if there are more points than maximum stars, pretend to be confused. 
    if (points > MAXSTARS) {
      (*dx)=0;
      (*dy)=0;
      (*drot)=0;
      return -1;
    };
    // first compute the shift between the two sets of stars
    for(i=0; i < points; i++) {
      if(valid[i] && use[i]){
	weight[i] = 1;
	sumweight += weight[i];
	sumx += weight[i]*(x1[i]-x0[i]);
	sumy += weight[i]*(y1[i]-y0[i]);
	cx += x0[i];
	cy += y0[i];
	cx1 += x1[i];
	cy1 += y1[i];
	count++;
      }; // if(valid)
    }; // for i
    // populate the dx and dy
    (*dx) = sumx/sumweight;
    (*dy) = sumy/sumweight;
    cx = cx/count;
    cy = cy/count;
    cx1 = cx1/count;
    cy1 = cy1/count;
    // now the rotation.
    for(i=0; i < points; i++ ){
      if(valid[i] && use[i]) {
	// residual offset between the two points
	//ux = x1[i]-x0[i]-(*dx);
	//uy = (y1[i]-y0[i]-(*dy))*XY_RATIO;
        // vector from centroid to centerpont between stars
	//wx = (x1[i]-(*dx)+x0[i])/2.0-cx;
	//wy = ((y1[i]-(*dy)+y0[i])/2.0-cy)*XY_RATIO;
	// cross product / w.w (gives atan of the angle, pretty much)
	crossp = (ux*wy - uy*wx)/(wx*wx+wy*wy);
	// different
	ux = x0[i]-cx;
	uy = y0[i]-cy;
	wx = (x1[i]-(*dx))-cx;
	wy = (y1[i]-(*dy))-cy;
	
	ang = asin((ux*wy-wx*uy)/sqrt((ux*ux+uy*uy)*(wx*wx+wy*wy)));
	sumrot += weight[i]*ang;
	// "convert" to angle using first two terms of arcttan Taylor
	// expansion and apply weights.
	//sumrot += weight[i] * (crossp-crossp*crossp*crossp/3.0);
      }; // if(valid)
    }; // for i
    // normalize and convert to degrees         
    tmp = sumrot/sumweight;
    if (((tmp*180/M_PI) <200)&& ((tmp*180/M_PI) > (-200))) {
      (*drot) = tmp*180/M_PI;
      // if there is a rotation, we need to adjust the translation
      // to the nominal center of rotation point.
      (*dx) = -(cx-(cx1-guidedata.rx)*cos(tmp)-(cy1-guidedata.ry)*sin(tmp)-guidedata.rx);
      (*dy) = -(cy+(cx1-guidedata.rx)*sin(tmp)-(cy1-guidedata.ry)*cos(tmp)-guidedata.ry);
	//      (*dx) += (cx-guidedata.rx)*cos(tmp)-(cy-guidedata.ry)*sin(tmp)
	//	-(cx-guidedata.rx);
      //     (*dy) += (cx-guidedata.rx)*sin(tmp)+(cy-guidedata.ry)*cos(tmp)
      //	-(cy-guidedata.ry);

      
    } else {
      (*drot) = 0.0;
      // if there is no rotation, the translation is adequate. 
    };
    return 0;
  }; // compute_mapping


  void set_target_points(short *tx, 
			 short *ty){
    float ntx[MAXSTARS],nty[MAXSTARS],ntf[MAXSTARS];
    unsigned char nuse[MAXSTARS];
    int ctr=0;
    for(int i=0;i<MAXSTARS; i++){
      if(tx[i] > 0 && ty[i] > 0){
	ntx[i] = tx[i]*1.0;
	nty[i] = ty[i]*1.0;
	nuse[i] = 1;
	ntf[i]=-1;
	ctr++;
      } else {
	nuse[i] = -1;
	ntf[i] = -1;
      };// if
    }; //i
    if(ctr>0){
      memcpy(guidedata.nexttx, ntx,MAXSTARS*sizeof(float));
      memcpy(guidedata.nextty, nty,MAXSTARS*sizeof(float));
      memcpy(guidedata.nexttf, ntf,MAXSTARS*sizeof(float));
      memcpy(guidedata.nexttw, ntf,MAXSTARS*sizeof(float));
      memcpy(guidedata.nextuse, nuse, MAXSTARS);
    };
    return;
  }; //set_target_points()

  // rotation happens first, then translation
  // rotation is in degrees, translation is in detector pixels
  void transform_target_points(float dx, float dy, float rot){
    float tmpx, tmpy, rotr;
    float tx[MAXSTARS], ty[MAXSTARS];
    remove_distortion(guidedata.tx, guidedata.ty, tx, ty, MAXSTARS);
    int i;
    rotr = rot*M_PI/180.;
    for(i=0; i<MAXSTARS; i++){
      tmpx = (guidedata.tx[i]-guidedata.rx)+
      (guidedata.tx[i]-guidedata.rx)*cos(rotr)
      -(guidedata.ty[i]-guidedata.ry)*sin(rotr);

      tmpy = (guidedata.ty[i]-guidedata.ry)+
      (guidedata.ty[i]-guidedata.ry)*cos(rotr)
      +(guidedata.tx[i]-guidedata.rx)*sin(rotr);
      
      guidedata.tx[i] = tmpx + dx;
      guidedata.ty[i] = tmpy + dy;
    }; // for i 
    return;
  };


  // there might be some distortion in the field.
  void remove_distortion(float *x, float *y,
			 float *v, float *u,
			 int count){

    return;
  }; 

  void add_distortion(float *u, float *v,
		      float *x, float *y,
		      int count){

    return;
  };


  inline void set_rotation_point(float rx, float ry){
    guidedata.rx = rx;
    guidedata.ry = ry;
    return;
  }; // set_rotation_point

  inline void get_rotation_point(float rx, float ry){
    rx = guidedata.rx;
    ry = guidedata.ry;
    return;
  }; // get_rotation_point

  void load_next_target(unsigned short fileno){
    // construct filename to load
    // check file exists
    // load file, making sure valid
    float x[MAXSTARS], y[MAXSTARS];
    unsigned char valid[MAXSTARS];
    double ra, dec, pa;
    int stars, cnt, retcnt;
    size_t len;
    FILE *fp;
    char fname[255];
    char line[255];
    char **lin;
    int err, ctr;
    

    sprintf(fname,"/home/salo/FB/Fireball2/fbguider/targets/target%05d.fb",fileno);
    err = 0;
    stars =0;
    lin = ( char **) &(line[0]);
    if( access( fname, R_OK ) != -1 ) {
      // file exists
      // open file
      fp = fopen(fname,"r");
      //RA
      retcnt = fscanf(fp,"RA %lf \n",&ra);
      if ( retcnt != 1) err = 1;
      //Dec
      retcnt = fscanf(fp, "Dec %lf \n",&dec);
      if ( retcnt != 1) err = 1;
      //PA
      retcnt = fscanf(fp,"PA %lf \n",&pa);
      if ( retcnt != 1) err = 1;
      // Stars
      retcnt = fscanf(fp,"N %d\n",&stars);
      if ( retcnt != 1) err = 1;

      fblog::logmsg("RA = %lf",ra);
      fblog::logmsg("Dec = %lf",dec);
      fblog::logmsg("PA = %lf",pa);
      fblog::logmsg("Stars = %d",stars);
      
      for(ctr=0;ctr<MAXSTARS;ctr++) valid[ctr]=0;

      if (stars >0 && stars <= MAXSTARS){
	for(ctr=0;ctr<stars;ctr++){
	  retcnt = fscanf(fp,"%f %f",x+ctr,y+ctr);
	  if ( retcnt == 2){
	    valid[ctr] = 1;
	    fblog::logmsg("Star%d %f %f",ctr,x[ctr],y[ctr]);
	  } else {
	    err = 1;
	  };
	};
      };
	//Position pairs.
	// close file
      fclose(fp);
      
      if(!err){
	set_next_targets(x,y,valid,ra,dec,pa);
	fblog::logmsg("Setting next target.");
      } else {
	fblog::logmsg("Errors found. Not setting next target");
      };// !err

    } else {
      // file doesn't exist
    };
      return;
  }; // load_next_target();

  // save current targets, RA, and dec.
  // this needs to be developed?
  void save_targets(unsigned short fileno){
    char fname[255];
  };


  void convert_next_target_to_next_star(float dx, float dy, float drot){ 
    // take the next target values and tell the guider
    // where the stars that are supposed to go into them are
    // Note that this need not take intou account the distortions,
    // unless they are HUGE, since this is just to grab a hold of the
    // stars with some rather large boxes...
    // note that this makes no use of RA, DEC...
    int n;
    float crot,srot, tempx, tempy, nextx, nexty;
    crot = cos(drot);
    srot = sin(drot);

    for(n=0;n<MAXSTARS;n++){
      nextx = guidedata.nexttx[n];
      nexty = guidedata.nextty[n];
      if(guidedata.nextuse[n]){
	tempx =  guidedata.rx + dx + (nextx-guidedata.rx)*crot - (nexty-guidedata.ry)*srot;
	tempy =  guidedata.ry + dy + (nexty-guidedata.rx)*srot + (nexty-guidedata.ry)*crot;
	guidedata.nextx[n] = tempx;
	guidedata.nexty[n] = tempy;
	guidedata.nextvalid[n]=1;
      } else {
	guidedata.nextvalid[n]=0;
      }; 
    }; //n 
    return;
  }; //next_target_to_next_star();

  void convert_next_target_to_next_star_sky(double ra, double dec, float pa) {
    // take the specified target values and use astrometry data to
    // identify where the stars are supposed to be.
    return; 
  }; // next_target_to_next_star_sky();
  
  // switch next values to current guide targets
  void switch_to_next_targets(){
    set_change_flag(1,0,0,0);
    return; 
  }; // switch_next_to_current();

  void switch_to_next_stars(){
    // 
    set_change_flag(0,1,0,0);
    return;
  }; // switch_to_next_stars();

  void update_targets(){
    set_change_flag(0,0,1,0);
    return;
  }; // update_target();

  void update_stars(){
    set_change_flag(0,0,0,1);
  };// update_stars();



  // this does precious little error checking.
  void set_next_targets(float *sx, float *sy, unsigned char *suse, double ra, double dec, double rot){
    memcpy(guidedata.nexttx, sx, MAXSTARS*sizeof(float));
    memcpy(guidedata.nextty, sy, MAXSTARS*sizeof(float));
    memcpy(guidedata.nextuse, suse, MAXSTARS);
    for(int n=0;n<MAXSTARS;n++){
      guidedata.nexttf[n] = -1;
      guidedata.nexttw[n] = -1;
    };
    guidedata.next_field_ra = ra;
    guidedata.next_field_dec = dec;
    guidedata.next_field_pa = rot;
    return; 
  };

  void set_next_single(float sx, float sy, unsigned char suse){
    static float x[MAXSTARS],y[MAXSTARS];
    static unsigned char use[MAXSTARS];
    
    if(sx>0 && sx<guidedata.width && sy>0 && sy<guidedata.height){
      x[0] = sx; y[0] = sy;
      use[0] = suse;
      for(int n=1;n<MAXSTARS;n++){
	x[n]=0; y[n] =0; use[n]=0;
      };
      set_next_targets(x,y,use,0,0,0);
      set_next_stars(x,y,use);
      switch_to_next_targets();
      switch_to_next_stars();
    }; // in bounds
    return;
  }; // set_next_target_single

  void set_next_stars(float *sx, float *sy, unsigned char *svalid){
    memcpy(guidedata.nextx, sx, MAXSTARS*sizeof(float));
    memcpy(guidedata.nexty, sy, MAXSTARS*sizeof(float));
    memcpy(guidedata.nextvalid, svalid, MAXSTARS);

    for(int n=0;n<MAXSTARS;n++){
      guidedata.nextf[n]=-1;
    };

    return;
    
  }; //set_next_stars();
  
  // adjusts the current targets in detector coordinates?
  // focal plane coordinates? tbd, really..
  void adjust_targets(float dx, float dy, float drot){
    int n;
    float crot,srot, tempx, tempy, nextx, nexty;
    crot = cos(drot);
    srot = sin(drot);

    fblog::logmsg("Requested target adjust: %f %f %f %f %f",dx,dy,drot,guidedata.rx,guidedata.ry);

    for(n=0;n<MAXSTARS;n++){
      nextx = guidedata.tx[n];
      nexty = guidedata.ty[n];
      tempx =  guidedata.rx + dx + (nextx-guidedata.rx)*crot - (nexty-guidedata.ry)*srot;
      tempy =  guidedata.ry + dy + (nextx-guidedata.rx)*srot + (nexty-guidedata.ry)*crot;
      guidedata.nexttx[n] = tempx;
      guidedata.nextty[n] = tempy;
      if(tempx < guidedata.width-1 && tempx >0 && tempy<guidedata.height-1 && tempy > 0 && guidedata.use[n]){
	guidedata.nextuse[n] = 1;
      } else { 
	guidedata.nextuse[n] = 0;
      };
    }; //n 
    
    return; 
  };

  void set_change_flag(char nxtargs, char nxstars, char updtargs, char updstars){
    if(nxtargs) guidedata.nexttargets = 1;
    if(nxstars) guidedata.nextstars = 1;
    if(updtargs) guidedata.updatetargets = 1;
    if(updstars) guidedata.updatestars = 1;
    return;
  };




  void compute_elce(double specrot_radians, 
		     float specrot_radians_offset,
		     float dx,
		     float dy,
		     float *el,
		     float *ce){
    static float srot, crot;
    srot = sin(specrot_radians-specrot_radians_offset);
    crot = cos(specrot_radians-specrot_radians_offset);
    (*el) = (crot*dx*PLATESCALEX - srot*dy*PLATESCALEY);
    (*ce) = (srot*dx*PLATESCALEX - crot*dy*PLATESCALEY);
    return;
  }; // compute_el_ce


  void set_attitude_sensor(char sensor){
    switch(sensor){
    case 1:
      attitude.ra = &(attitude.ra_a);
      attitude.dec = &(attitude.dec_a);
      attitude.pa = &(attitude.pa_a);
      break;
    case 2:
      attitude.ra = &(attitude.ra_g);
      attitude.dec = &(attitude.dec_g);
      attitude.pa = &(attitude.pa_g);
      break;
    default:
      attitude.ra = &(attitude.ra_d);
      attitude.dec = &(attitude.dec_d);
      attitude.pa = &(attitude.pa_d);
      break;
    }; // switch(sensor)
    return;
  }; // set_attitude_sensor();
  
  void upload_attitude(double ra_, double dec_, double pa_){
    attitude.ra_g = ra_;
    attitude.dec_g = dec_;
    attitude.pa_g = pa_;
    return;
  }; // upload attitude

  void gondola_attitude(double ra_, double dec_, double pa_){
    attitude.ra_d = ra_;
    attitude.dec_d = dec_;
    attitude.pa_d = pa_;
    return;
  }; // gondola_attitude

  void astrometry_attitude(double ra_, double dec_, double pa_){
    attitude.ra_a = ra_;
    attitude.dec_a = dec_;
    attitude.pa_a = pa_;
    return;
  }; // astrometry_attitude
  
}; // namespace img

