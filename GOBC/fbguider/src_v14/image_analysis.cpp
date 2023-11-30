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
#include "rotation.h"
#include "fit.h"

#include "motors.h"

// for frameblobs.
#include "frameblob.h"
#include "bloblist.h"

#include "fitsio.h"

namespace img{
  // variables
  

  
  FBModes fbmode;
  boxes_t boxes;
  guide_data_t guidedata;
  attitude_t attitude;

  float offval[5]; //adding sx, sy here
  float offrms[3];

  bloblist *pBlobs[IMAGE_BUFFERS];
  frameblob *fblob;

  // image boxsize, constant 
  const int img_boxsize=get_env_boxsize();

  int badcounter;

  pthread_t imageworker_thread[IMAGE_BUFFERS];
  
  pthread_mutex_t imageworker_mutex=PTHREAD_MUTEX_INITIALIZER;

  uint16_t medbuff[1280*1080L];
  int do_median;
  int ct_median;
  int maxct_median;
  long median_first;

  
  int saveperiod;
  
  int subtract;
  unsigned char algorithm;
  //  uint16_t imgstack[1280*1080*STACKSIZE];
  
  int do_history;
  float cx_history[MAXSTARS];
  float cy_history[MAXSTARS];
  int ct_history[MAXSTARS];
  unsigned char ct_history_max;

  unsigned short imagetag;
  static char msg[255];
  star_profile_t curprofile[MAXSTARS];

  // functions

  int imgboxsize(){
    return img_boxsize;
  };

  int get_env_boxsize(){
    char *value;
    int bsize;
    value = getenv("IMGBOXSIZE");
    if ( value == NULL ){
      fblog::logmsg("Using default box size: %d",IMG_BOXSIZE_DEFAULT);
      return IMG_BOXSIZE_DEFAULT;
    } else {
      bsize = atoi(value);
      if (bsize < IMG_BOXSIZE_MIN){
	fblog::logmsg("Using min box size: %d",IMG_BOXSIZE_MIN);
	return IMG_BOXSIZE_MIN;
      };
      if ( bsize > IMG_BOXSIZE_MAX){
	fblog::logmsg("Using max box size: %d",IMG_BOXSIZE_MAX);
	return IMG_BOXSIZE_MAX;
    };
      fblog::logmsg("Using box size: %d",bsize);
      return bsize;
    }
    return IMG_BOXSIZE_DEFAULT;
  };

  void process_frame(camera_parameters_t *campar, int bufidx){
    // fill out the guide parameters here...
    // fire off the worker thread!
    guidedata.buf = (*campar).picbuf[bufidx];
    guidedata.bufidx = bufidx;
    guidedata.framenumber = (*campar).framenumber;
    pthread_create(&imageworker_thread[bufidx], NULL, imageworker, NULL);
    pthread_detach(imageworker_thread[bufidx]);
    DEBUGLINE;
  };

  // initialize the guide data structure.
  void imginit(int w, int h,int boxsize){
    // initialize boxes
    init_boxes(boxsize);

    // initialize guide data
    guidedata.snrcut = IMG_SNR;
    guidedata.star_sig_min= STAR_SIG_MIN;
    guidedata.star_sig_max= STAR_SIG_MAX;
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


    guidedata.maxstars = MAXSTARS;


    // initialize frameblobs
    fprintf(stderr,"INITIALIZING BLOBS\n");
    guidedata.fb_bits = FB_BITS;
    guidedata.fb_cam_sat=FB_CAM_SAT;
    guidedata.fb_plate_scale=FB_PLATE_SCALE;
    guidedata.fb_coarse_grid=FB_COARSE_GRID;
    guidedata.fb_cen_box=FB_CEN_BOX;
    guidedata.fb_ap_box=FB_AP_BOX;
    guidedata.fb_sigma_thresh=FB_SIGMA_THRESH;
    guidedata.fb_dist_tol=FB_DIST_TOL;
    guidedata.fb_rn=FB_RN;
    guidedata.fb_offset=FB_OFFSET;
    guidedata.fb_gain=FB_GAIN;

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
      fblob[idx].set_disttol(((int)guidedata.fb_dist_tol)*guidedata.fb_dist_tol);
      fblob[idx].set_gain(guidedata.fb_gain);
      fblob[idx].set_readout_offset(guidedata.fb_offset);
      fblob[idx].set_readout_noise(guidedata.fb_rn);
      
    };// for idx

    subtract=3;
    algorithm=2;
    load_tagnumber();
    ct_history_max=100;
    saveperiod=IMG_SAVE_PERIOD;

    do_median=0;
    ct_median=0;
    maxct_median=100;
			   
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

  void extract_boxes(uint16_t *buf, float *cx, float *cy, unsigned char *success, unsigned char *valid){
    static int idx, jdx,x,y;
    static uint16_t *ptrs, *ptrt;
    //memset(success,0,MAXSTARS);
      if(boxes.initialized && guidedata.initialized){
	// loop over boxes
	for(idx=0;idx<MAXSTARS;idx++){
	  // only extract the ones we are asked to...
	  if(valid[idx]) {
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
	    } else {
	      success[idx] = FALSE;
	    }; // within bounds
	  }; // use?
	}; // for(idx)
      }; // boxes&&guidedata initialized
    return;
  }; // extract_boxes();

  void centroid_boxes_1(float *cx, float *cy, float *cf, float *sigx, float *sigy, unsigned char *valid, unsigned char *use){

    // pseudocode:
    // set up frameblobs
    // loop over valid stars 
    // clear frameblobs
    // centroid the box, populate the structures
    // exit 
    static bloblist *pb;
    static frameblob *fb;
    static char initialized=0;
    static int idx,stars_found;
    static uint16_t *buf;
    //    static unsigned int xfb,yfb;

    if ( ( boxes.initialized ) && (guidedata.initialized) ){
      
      
      frameblob fblob(boxes.boxes[0],boxes.size,boxes.size,16,1.0);
      fb=&fblob;
      
      if (! initialized ) {
	fb->set_satval(guidedata.fb_cam_sat);
	fb->set_grid(guidedata.fb_coarse_grid);
	fb->set_cenbox(guidedata.fb_cen_box);
	fb->set_apbox(guidedata.fb_ap_box);
	fb->set_threshold(guidedata.fb_sigma_thresh);
	fb->set_disttol(guidedata.fb_dist_tol*guidedata.fb_dist_tol);
	initialized=1;
      };
      
      for(idx=0;idx<MAXSTARS;idx++){
	if (guidedata.valid[idx]){
	  
	  // tell it where to look.
	  buf=boxes.boxes[idx];
	  fb->setMap(buf);
	  
	  // clear the blobs
	  fb->clearblobs();
	  
	  // find the blobs.
	  fb->calc_mapstat();
	  fb->calc_mapstat();
	  fb->calc_searchgrid();
	  fb->calc_centroid();
	  fb->calc_flux();
	  fb->fix_multiple();
	  fb->sortblobs();
	
	stars_found = fb->Numblobs();
	
	if (stars_found > 0 ) {
	  pb = fb->getblobs();
	  cx[idx]= pb->getx()+boxes.cx[idx]+1-boxes.size/2;
	  cy[idx]= pb->gety()+boxes.cy[idx]+1-boxes.size/2;
	  cf[idx]= pb->getflux();
	  sigx[idx] = (float) fb->computeFocusX();
	  sigy[idx] = (float) fb->computeFocusY();
	  printf("CTR %d %.2lf %.2lf %.2lf %.2lf %.2lf %d\n",idx,cx[idx],cy[idx],cf[idx],sigx[idx],sigy[idx],boxes.size);
	  
	  if ( (cx[idx] > 0 ) && (cy[idx]>0) &&
	       (cx[idx] < 1280 ) && (cy[idx] < 1080)){
	    valid[idx]=1;
	  } else {
	    valid[idx]=0;
	    use[idx]=0;
	    printf("This one!\n");
	  };
	} else { 
	  printf("No stars!\n");
	  valid[idx]=0;
	};
	

      }; // valid 
    }; // iloop
    
    }; // boxes and guidedata initialized
   
    return;
  }; //centroid_boxes1

  //LM centroiding...
  void centroid_boxes_2(float *cx, float *cy, float *cf, float *sigx, float *sigy, unsigned char *valid, unsigned char *use){
    // pseudo-code
    // collapse box in x or y, find loudest pixel
    // collapse 2n+1 rows around loudest pixel, generate 1-d array
    // collapse 2n+1 columns around loudest row, generate 1-d array
    // fit gaussians to both
    // reject answers based on bad sigmas or 

    static int idx,curidx;
    static int rowmax, colmax,valctr;
    static int im,in;
    static float summ[200],sumn[200];
    static float p[4],q[4];
    static float maxm,maxn,minm,minn;
    static float chip,chiq;
    static int midx, nidx ,mmidx, nmidx;
    static float bg,flux,area,snr,amplitude;
    //static float allsnr[MAXSTARS];                                                                                  
    //    unsigned char curval;                                                                                       
    //    DEBUGPRINT("centroid: %d\n",count++);                                                                       
    rowmax = colmax = -1;
    if(boxes.initialized && guidedata.initialized){
      for(idx=0;idx<MAXSTARS;idx++){
        // only compute centroid if we want to use this box.                                                          
	//	curval=valid[idx];                                                                                    

        if(use[idx]){
          memset(summ,0,200*sizeof(float));
          memset(sumn,0,200*sizeof(float));

          for (im=0;im<boxes.size;im++){
            for (in=0;in<boxes.size;in++){
              summ[im]+=boxes.boxes[idx][im+in*boxes.size];
              sumn[im]+=boxes.boxes[idx][in+im*boxes.size];
            }; //in                                                                                                   
          }; // im                                                                                    
	  //void find_minmax(float *xprof, float *yprof, int size, int smooth, float *xmin, int *xminidx, float *xmax, float *xmaxidx, float *ymin, float *yminidx, float *ymax, float *ymaxidx){                

	  find_minmax(summ, sumn, IMG_BOXSIZE*2+1, 3, 
		      &minm, &mmidx,
		      &maxm, &midx,
		      &minn, &nmidx,
		      &maxn, &nidx);

	  // row and column of the maximum
	  rowmax=nidx;
	  colmax=midx;
	  
	  //	  printf("MINMAX %d %d\n");
	  
	  // second pass
          memset(summ,0,200*sizeof(float));
          memset(sumn,0,200*sizeof(float));

          for (im=0;im<boxes.size;im++){
            for (in=-boxes.star_cut;in<=boxes.star_cut;in++){
	      curidx=in+rowmax;
	      if( curidx >=0 && curidx < boxes.size)
		summ[im]+=boxes.boxes[idx][im+curidx*boxes.size];
	      
	      curidx=in+colmax;
	      if (curidx >=0 && curidx < boxes.size)
		sumn[im]+=boxes.boxes[idx][curidx+im*boxes.size];
            }; //in                                                                                                  
          }; // im                                                                                                    
	  find_minmax(summ, sumn, IMG_BOXSIZE*2+1, 3, 
		      &minm, &mmidx,
		      &maxm, &midx,
		      &minn, &nmidx,
		      &maxn, &nidx);
	  // p corresponds to m
	  // q corresponds to n
	  
	  // row and column of the maximum

	  p[0]=maxm-minm;
	  p[1]=midx;
	  p[2]=boxes.star_cut;
	  p[3]=minm;
	  
	  q[0]=maxn-minn;
	  q[1]=nidx;
	  q[2]=boxes.star_cut;
	  q[3]=minn;

	  // magic.
	  fit::find_gauss_centroids(&summ[0],&p[0],&chip,&sumn[0],&q[0],&chiq,boxes.size,4);

	  // printf("Star %d use %d\n",idx,use[idx]);
	  // for(im=0;im<4;im++)
	  //   printf("%lf\t%lf\n",p[im],q[im]);
	  //   printf("%lf\t%lf\n",chip,chiq);
	  //   printf("%d\t%d\n",colmax,rowmax);


	  // test conditions
	  valctr=0;
	  if ( ( p[1] < 4.0 ) || ( p[1] > boxes.size-4 ) ) {
	    valctr++;
	    //	    printf("%d Bad p center %f\n",idx,p[1]);
	  }
	  if ( ( q[1] < 4.0 ) || ( q[1] > boxes.size-4 ) ) {
	    valctr++;
	    //	    printf("%d Bad q center %f\n",idx,q[1]);
	  }
	  if ( ( p[2] > guidedata.star_sig_max ) || 
	       ( q[2] > guidedata.star_sig_max )  || 
	       ( p[2] < guidedata.star_sig_min ) || 
	       ( q[2] < guidedata.star_sig_min ) ) {
	    valctr++;
	    //	    printf("%d Bad sigma %f %f\n",idx,p[2],q[2]);
	  };

	  area=p[2]*q[2];
	  amplitude=p[0];
	  if (q[0] < p[0]) amplitude=q[0];
	  flux = 6.28*amplitude*area/boxes.size;
	  bg= sqrt(20.0*area);
	  snr=flux/bg;
	  if (snr < 3.0){
	    valctr++;
	    // printf("%d Bad SNR %f %f %f %f\n",idx,flux,bg,flux/bg,area);
	  };
	  //	  printf("SNR: %f\n",snr);
	  //	  printf("\n");
	  // some condition on chi sq
	  
	  if ( ! valctr ){ 
	    // OMG WE HAVE A VALID STAR!!!
	    // it has a flux, and everything!
	    valid[idx]=1;
	    sigx[idx]=p[2];
	    cf[idx]=(p[0]+q[0])/2.0*p[2]*q[2];
	    cx[idx]=p[1]+boxes.cx[idx]-boxes.size/2;
	    cy[idx]=q[1]+boxes.cy[idx]-boxes.size/2;
	    cf[idx]=sqrt(p[0]*q[0])*p[2]*q[2];
	    sigx[idx]=p[2];
	    sigy[idx]=q[2];
	    //	    printf("%d profile!\n",idx);
	    set_profiles(idx, valid[idx], use[idx],
			 summ, sumn, p,q, getFramenumber());
	    
	    //  void set_profiles(unsigned char starid, unsigned char val, unsigned char used, float *xprofile, float *yprofile, float *xfit, float *yfit,long frameno){
	    
	      
	  } else {
	    //printf("%d Failed star %d\n",idx,valctr);
	    valid[idx]=0;
	  };

	}; // are the stars used?
      }; // loop over stars
    }; // boxes initialized
  }; // centroid_boxes2



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
    static int im,in;
    static float summ[200],sumn[200];
    static float maxm,maxn;
    static int midx, nidx;
    //static float allsnr[MAXSTARS]; 
    //    unsigned char curval;
    //    DEBUGPRINT("centroid: %d\n",count++);
    max = rowmax = colmax = -1;
    if(boxes.initialized && guidedata.initialized){
      for(idx=0;idx<MAXSTARS;idx++){
	// only compute centroid if we want to use this box.
	//	curval=valid[idx];
	
	if(use[idx]){
	  //	  valid[idx]=0;

	  
	  memset(summ,0,200*sizeof(float));
	  memset(sumn,0,200*sizeof(float));

	  for (im=0;im<boxes.size;im++){
	    for (in=0;in<boxes.size;in++){
	      summ[im]+=boxes.boxes[idx][im+in*boxes.size];
	      sumn[im]+=boxes.boxes[idx][in+im*boxes.size];
	    }; //in
	  }; // im

	  maxm=0;
	  maxn=0;
	  midx=0;
	  nidx=0;
	  for(im=0;im<boxes.size;im++){
	    if (summ[im]>maxm){
	      maxm=summ[im];
	      midx=im;
	    }
	    if (sumn[im]>maxn){
	      maxn=sumn[im];
	      nidx=im;
	    };
	  };
	  
	  rowmax=nidx;
	  colmax=midx;



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
	  //	  printf("max: %d %d %d\n",rowmax,colmax,max);

	  	  //DEBUGPRINT("max: %d %d %d\n",rowmax,colmax,max);

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
	    //allsnr[idx] = snr; 
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
	    //	    printf("snr %5.2f \n", snr); 
	    // 170727 -- matmat
	    // if (snr > IMG_SNR && sig_x < STAR_SIG_MAX && sig_y < STAR_SIG_MAX && sig_x > STAR_SIG_MIN && sig_y> STAR_SIG_MIN ){
	    //guidedata.snrcut = 1.0;
	    //	    printf("SNRS %d %lf %lf %lf %lf %lf %lf %lf %lf %d %d %d %d %d %d",idx, snr,
	    //	   guidedata.snrcut, sig_x,guidedata.star_sig_max, sig_y,
	    //	   guidedata.star_sig_min,
	    //	   c_x+colmax-boxes.size/2.0+boxes.cx[idx]+1,
	    //	   c_y+rowmax-boxes.size/2.0+boxes.cy[idx]+1,
	    //	   valid[idx],use[idx],boxes.cx[idx],boxes.cy[idx],rowmax,colmax);
	    if (snr > guidedata.snrcut && sig_x < guidedata.star_sig_max && sig_y < guidedata.star_sig_max && sig_x > guidedata.star_sig_min && sig_y> guidedata.star_sig_min ){
	      c_x = c_x+colmax-boxes.size/2.0+boxes.cx[idx]+1;
	      c_y = c_y+rowmax-boxes.size/2.0+boxes.cy[idx]+1;
	      //printf("SNR:: %d %5.2lf %5.2f %5.2f %5.2lf %5.2lf %.2lf %.2lf",idx,snr, IMG_SNR ,fgsum, noise, bg_level, c_x, c_y);
	      if ( c_x > boxes.halfwidth && c_y > boxes.halfwidth && c_x < guidedata.width-boxes.halfwidth-1 && c_y < guidedata.height-boxes.halfwidth-1 ){
		valid[idx] = 1;
		cf[idx] = fgsum;
		cx[idx] = c_x;
		cy[idx] = c_y;
		sigx[idx] = sig_x;
		sigy[idx] = sig_y;

	      }; // star is not near the edge. 
	      //	      
	    }; // good snr
	  
	  }; // centroid in box
      
      //if(valid[idx] == 0){
	  //	  printf("SNR:: %d %5.2lf %5.2f %5.2f %5.2f %5.2f %5.2lf %5.2lf\n",idx,snr[idx], sigx[idx], sigy[idx], IMG_SNR , cf[idx], noise, bg_level);
      //	}
	  //printf(" %d\n",valid[idx]);
	};//use[idx];
     
       if(valid[idx] == 0) {
          //printf("SNR:: %d %5.2lf %5.2f %5.2f %5.2f %5.2f %5.2lf %5.2lf\n",idx,allsnr[idx], sigx[idx], sigy[idx], IMG_SNR , cf[idx], noise, bg_level);
	  }
      };//for(idx)
      //      printf("\n");
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
      do_history=0;
      };
    return;
  }; //setFBMode();
 
  void mag_func(float *vmag, float *flux, int& my_exposure){
	float eff = 1.0; // from Excel Document 
	for(int i=0; i<MAXSTARS;i++){
		vmag[i] = 12.5 + (log10(flux[i]/(my_exposure*3.0*eff*800.))/(-0.4)); 
	}; 
};
  
  void *imageworker(void *data){
    static guide_data_t *gd;
    static FBModes fbm;
    static unsigned char nstars;
    static float x[MAXSTARS],y[MAXSTARS],f[MAXSTARS];
    static double xfwhm, yfwhm; 
    static unsigned char valid[MAXSTARS];
    static float sigx[MAXSTARS], sigy[MAXSTARS];
    static int idx, valcnt;
    static float dx, dy, drot, pa, lastpa;
    static float dEL, dCE;
    static float main_sigx, main_sigy;
    //    static float lastdx, lastdy; //, lastdrot;
    static float gel,gaz,grot;


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
    main_sigx = main_sigy = 0.0;

    //fblog::logmsg("Processing frame: %ld",gd->framenumber);

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
    
    if(do_median){
      construct_min_image(gd->buf);
    };
    
    if (subtract & 0x01 ){
      subtract_median(gd->buf);
    };
    if (subtract & 0x02 ){
      subtract_rownoise(gd->buf);
    };

    // giant switch that decides what to do, depending on the mode.
    switch(fbm){
    case Idle:
      // do nothing -- just display later
      break;
    case Acquire:
      //acq_func(gd->buf, gd->x, gd->y, gd->f, gd->valid, &nstars, gd->bufidx);
      acq_func(gd->buf, x, y, f, valid, &nstars, gd->bufidx, &xfwhm, &yfwhm);
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
      memset(guidedata.use,0,MAXSTARS);
      for(idx=0;idx<guidedata.maxstars;idx++){
	//	guidedata.valid[idx] = 1;
	guidedata.use[idx] = 1;
	guidedata.tw[idx] = log(guidedata.tf[idx]);
      }; // idx

      break;
    case MultiStarFixed:
      // change 170831
      //extract_boxes(gd->buf, gd->tx,gd->ty, valid, guidedata.valid);
      extract_boxes(gd->buf, gd->tx,gd->ty, valid, guidedata.use);

      if(algorithm == 0){
	centroid_boxes(x,y,f, sigx, sigy, valid, guidedata.use);
      } else if (algorithm == 1) {
	centroid_boxes_1(x,y,f, sigx, sigy, valid, guidedata.use);
      } else {
	centroid_boxes_2(x,y,f, sigx, sigy, valid, guidedata.use);
      };

      
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
	  guidedata.sigx[idx]=sigx[idx];
	  guidedata.sigy[idx]=sigy[idx];
    if(guidedata.use[idx] == 1 && main_sigx == 0.0){
      main_sigx = sigx[idx];
      main_sigy = sigy[idx];
    }
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
      // set guidedata -- nicole 
      guidedata.dx = dx; 
      guidedata.dy = dy; 
      guidedata.drot = drot;

      compile_guide_stats(dx,dy,drot,main_sigx,main_sigy);

      if (do_history){
	refine_targets();
      };
      
      // write binary structure to disk
      
      write_guide_data(&guidedata);
      write_guide_text_data(&guidedata);
      
      //      printf("--> %lf\t%6.2f %6.2f %6.2f %6.2f\t %6.2f %6.2f %6.2f %6.2f\t%6.2f %6.2f %6.2f %6.2f\t %6.2f %6.2f %7.3f\n",
      fblog::logmsg("--> %lf\t%6.2f %6.2f %6.2f %6.2f\t %6.2f %6.2f %6.2f %6.2f\t%6.2f %6.2f %6.2f %6.2f\t %6.2f %6.2f %7.3f",
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
      // changed 170831
      //extract_boxes(gd->buf, gd->x,gd->y, valid, guidedata.valid);
	
      extract_boxes(gd->buf, gd->x,gd->y, valid, guidedata.use);
      if(algorithm == 0){
	centroid_boxes(x,y,f, sigx, sigy, valid, guidedata.use);
      } else if (algorithm == 1) {
	centroid_boxes_1(x,y,f, sigx, sigy, valid, guidedata.use);
      } else {
	centroid_boxes_2(x,y,f, sigx, sigy, valid, guidedata.use);
      };
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
	  guidedata.sigx[idx]=sigx[idx];
	  guidedata.sigy[idx]=sigy[idx];

    if(guidedata.use[idx] == 1 && main_sigx == 0.0){
      main_sigx = sigx[idx];
      main_sigy = sigy[idx];
    }

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
      // set error values -- nicole
      guidedata.dx = dx; 
      guidedata.dy = dy; 
      guidedata.drot = drot; 

      
      
      float raddrot;
      raddrot = drot/180.*3.14159;
      for(idx = 0; idx< MAXSTARS; idx++){
	if (guidedata.valid[idx]==0 && guidedata.use[idx]==1) {
	  raddrot=0.0;
	  guidedata.x[idx] = guidedata.rx + (guidedata.tx[idx]-guidedata.rx)*cos(raddrot)-(guidedata.ty[idx]-guidedata.ry)*sin(raddrot)+dx;
	  guidedata.y[idx] = guidedata.ry + (guidedata.tx[idx]-guidedata.rx)*sin(raddrot)+(guidedata.ty[idx]-guidedata.ry)*cos(raddrot)+dy;
	};
      };
      
      if (do_history){
	refine_targets();
      };

      compile_guide_stats(dx,dy,drot, main_sigx, main_sigy);

      write_guide_data(&guidedata);
      write_guide_text_data(&guidedata);


      

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
	badcounter=0;
	//	lastdx = dx;
	//	lastdy = dy;
	//lastdrot = drot;
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
	// remove below if all is working
        //	pa = 0.0;
	compute_elce(pa, ROTATOR_OFFSET, dx, dy, &dEL, &dCE);
	// update the values
	if (valcnt > 1){
	  gondola::update_guider_rotation_error(drot,1);
	} else {
	  gondola::update_guider_rotation_error(0,0);
	};
	// write analog voltages (old)
	// adio::analog_out_both(dCE*VOLTSBINPIX, dEL*VOLTSBINPIX);

	// NEW ENCODING ALGORITHM v3 - June 2017 
	adio::analog_out_both(ge_ang2volts(dCE), ge_ang2volts(dEL));
	// send the rotation error. 

	gondola::send_guider_rotation_error();
	// compute the _current_ guider alpha/delta/pa based on the 
	// error and _guider_ understanding of position.
	
      } else { // valcnt<=0
	if (badcounter < BADCOUNTER){
	  // write analog voltages                                          
	  adio::analog_out_both(ge_ang2volts(dCE), ge_ang2volts(dEL));
	  // send the rotation error.             
	  gondola::update_guider_rotation_error(0,0);
	  gondola::send_guider_rotation_error();
	  sprintf(msg,"Warning nostars %d",badcounter);
	  ground::send_text(msg);
	  badcounter++;
	} else {
	  adio::analog_out_both(ge_ang2volts(0.00), ge_ang2volts(0.00));
	  
	  // send the rotation error.             
	  gondola::send_guider_rotation_error();
	  //     if (valcnt ==0)           
	  //Possible to switch this to Acquire for automatic retry        
	  guidedata.fbmode = Idle   ;
	  gondola::update_guider_status(0);
	  sprintf(msg,"Lost Lock Switching to Idle.");
	  ground::send_text(msg);
	};
      }; // valcnt <= 0

      break;
      // these are the not-doing-anything here modes
    case Idle:
    case Acquire:
    default:
      break;
    }; //switch fbm

    fbvideo::display_frame(gd,&boxes, fbm, &xfwhm, &yfwhm);
    pthread_mutex_unlock(&imageworker_mutex);
    return NULL;
  };
  

  void acq_func(uint16_t *buf, float *x, float *y, float *flux, unsigned char *valid, unsigned char *nstars, int index, double *xfwhm, double *yfwhm){
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
    *nstars = std::min((int) guidedata.maxstars, stars_found);
    fprintf(stderr,"Caught %d stars out of %d.\n",*nstars,guidedata.maxstars*1);
    //    *nstars = std::min(MAXSTARS, stars_found);
    //    DEBUGPRINT("stars %d\n",*nstars);
    
    for(i=0; i<*nstars; i++){
      x[i] = pb->getx();
      y[i] = pb->gety();
      flux[i] = pb->getflux();
      

      //      DEBUGPRINT("%d %f %f %f\n",i,x[i],y[i],flux[i]);
      valid[i] = 1;
      pb = pb->getnextblob();
    };

    double fwhmx; 
    double fwhmy; 
    fwhmx = (double) fb->computeFocusX(); 
    fwhmy = (double) fb->computeFocusY(); 
    *xfwhm = fwhmx;
    *yfwhm = fwhmy; 

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
    static float sumx, sumy, sumweight, sumrot; //, crossp;
    static float ux,uy,wx, wy, tmp, ang;
    cx =0; cy=0; count=0; cx1=0; cy1=0;
    sumx =0; sumy=0; sumweight=0;
    ux = 0; uy =0; wx=0; wy=0;
    sumrot = 0;// crossp = 0;
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
   // temporary hack
    // if ( points > 1 ) {
    //   int starctr;
    //   starctr=0;
    //   (*drot)=0;
    //   sumx=0;
    //   sumy=0;
    //   for (i=0;i<MAXSTARS;i++){

    //     if(valid[i] && use[i]){
    //       sumx+=x1[i]-x0[i];
    //      sumy+=y1[i]-y0[i];
    //       starctr++;
    // 	};
    //   };
    //   (*dx)=sumx/starctr;
    //   (*dy)=sumy/starctr;
    //   printf("%f %f\n",(*dx),(*dy));
    //   return 0;
    // };
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
	//crossp = (ux*wy - uy*wx)/(wx*wx+wy*wy);
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
      //(*drot) = tmp*180/M_PI;
      (*drot) = rotation(tmp*180/M_PI); 
      //(*drot)=0;
      tmp=0;
      // (*drot) = rotation(tmp*180/M_PI);
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
    int stars, retcnt;
    unsigned short fno, loadstars;
    //size_t len;
    FILE *fp;
    char fname[255];
    //char line[255];
    //char **lin;
    int err, ctr;
    
    if (fileno > 999) {
      fno=fileno-1000;
      loadstars=1;
    } else {
      fno=fileno;
      loadstars=0;
    };
    sprintf(fname,"/home/salo/FB/Fireball2/fbguider/targets/target%05d.fb",fno);
    err = 0;
    stars =0;
    //lin = ( char **) &(line[0]);
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
      fblog::logmsg("Fname: %s",fname);
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
	sprintf(fname,"Loaded target %d",fno);
	ground::send_text(fname);
	if (loadstars){
	  
	};
	
      } else {
	fblog::logmsg("Errors found. Not setting next target");
	sprintf(fname,"Failed to load target %d",fno);
	ground::send_text(fname);
      };// !err

    } else {
      // file doesn't exist
      sprintf(fname,"Failed to load target %d",fno);
      ground::send_text(fname);
    };
      return;
  }; // load_next_target();

  // save current targets, RA, and dec.
  // this needs to be developed?
  void save_targets(unsigned short fileno){
    //char fname[255];
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
  
  // 
  void adjust_single_star(unsigned char starid, float newx, float newy){
    static float dx,dy,dr=0.0;
    static char tmp[255];
    
    if( (starid < 0) || (starid > MAXSTARS) ) {
      fblog::logmsg("ADJS: bad starid %d",starid);
      return;
    };
    
    if ( guidedata.valid[starid] && guidedata.use[starid]){
      
      if ( ( newx < 0 ) ||  ( newx > 1280 ) || ( newy < 0) || (newy > 1080) ){
	fblog::logmsg("ADJS: bad coord %f %f",newx,newy);
	return;
      };
      
      dx=newx-guidedata.tx[starid];
      dy=newy-guidedata.ty[starid];
      
      img::adjust_targets(dx,dy,dr);
      
      img::switch_to_next_targets();

    } else { 
      fblog::logmsg("ADJS invalid star %d",starid);
      sprintf(tmp,"Invalid star %d",starid);
      ground::send_text(tmp);
    };
    

    return;
  };

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
    (*ce) = (-srot*dx*PLATESCALEX - crot*dy*PLATESCALEY);
    return;
  }; // compute_el_ce

  double ge_ang2volts(double dang) {

    double dang_pseudo,dang_pix;

    if (dang > GE_THETALIM) {
      dang_pseudo=GE_NLIM+(dang-GE_THETALIM)/GE_LSB2;
    } else if (dang < ((-1.0)*GE_THETALIM)) {
      dang_pseudo=(-1.0)*GE_NLIM + (dang+GE_THETALIM)/GE_LSB2;
    } else {
      dang_pseudo=dang/GE_LSB1;
    }
        
    dang_pix=(dang_pseudo+2048);
    
    if (dang_pix > 4095) {
      dang_pix = 4095;
    }
    if (dang_pix < 0) {
      dang_pix = 0;
    }
    
    return((dang_pix-2048)*VOLTCONV);
  }

  // 170727 -- matmat
  void change_used_stars(unsigned char usemask){
    unsigned char usearr[MAXSTARS];
    unsigned char tmpmask;
    int j;
    unsigned char useok=0;
    tmpmask=usemask;
    // don't change the use array if the value sent is 0...
    // we want to be able to use the star. 
    if (usemask > 0) {
      for(j=0;j<MAXSTARS;j++){
	usearr[j] = (unsigned char) (( tmpmask >> j) & 0x01);
      }; // for j
      // One more sanity check here, we want to make sure we have at
      // least on one valid star specified.
      for (j=0;j<MAXSTARS; j++){
	usearr[j] = usearr[j] * guidedata.valid[j];
	useok = useok + usearr[j];
      }; // for j
      // Update if there is at least one valid star.
      if(useok > 0){
	memcpy(guidedata.use, usearr, MAXSTARS);
      }; // useok > 0
    }; // usemask 
    return;
  }; // change_used_stars(usemask)
  
  // 170727 -- matmat
  // this only makes sense if fsigmin < fsigmax
  // may want to add other conditions later. 
  void change_guide_star_sigma(unsigned char thresh, unsigned short sigmin, unsigned short sigmax){
    float fthresh,fsigmin,fsigmax;
    fthresh=thresh/16.0;
    fsigmin=sigmin/2048.0;
    fsigmax=sigmax/2048.0;
    if( (fsigmax > fsigmin) && (fsigmin > 0.1) && (fsigmax < 33.0)) {
      guidedata.star_sig_min=fsigmin;
      guidedata.star_sig_max=fsigmax;
    }; // sigma tests 
    if (fthresh > 1.0){
      guidedata.snrcut = fthresh;
    }; // thresh test
    return;
  };

  // 170727 -- matmat
  void get_thrsig(unsigned char *thresh, unsigned short *smin, unsigned short *smax){
    (*thresh) = guidedata.snrcut*16;
    (*smin) = guidedata.star_sig_min*2048;
    (*smax) = guidedata.star_sig_max*2048;
    return;
  };

  // 170728 -- matmat
  void get_target_pointers(float *tx, float *ty){
    memcpy(tx,guidedata.tx,MAXSTARS*sizeof(float));
    memcpy(ty,guidedata.ty,MAXSTARS*sizeof(float));
    return;
  };

  void get_star_pointers(float *cx, float *cy, float *cf, float *sx, float *sy){
    memcpy(cx,guidedata.x,MAXSTARS*sizeof(float));
    memcpy(cy,guidedata.y,MAXSTARS*sizeof(float));
    memcpy(cf,guidedata.f,MAXSTARS*sizeof(float));
    memcpy(sx,guidedata.sigx,MAXSTARS*sizeof(float));
    memcpy(sy,guidedata.sigy,MAXSTARS*sizeof(float));
    return;
  }; 

  void get_star_usevalid(unsigned char *use, unsigned char *valid){
    memcpy(use,guidedata.use,MAXSTARS);
    memcpy(valid,guidedata.valid,MAXSTARS);
    return;
  };

  // access functions for frameblob information
  void set_fb_gain(float gain){
    guidedata.fb_gain = gain;
    fprintf(stderr,"GAINSET %lf\n",guidedata.fb_gain);
    return;
  }; // set_fb_gain

  void set_fb_satval(uint16_t satval){
    guidedata.fb_cam_sat=satval;
    return;
  }; //set_fb_satval

  void set_fb_thresh(float thresh){
    guidedata.fb_sigma_thresh = thresh;
    return;
  }; // set_fb_thresh

  void set_fb_distol(int distol){
    guidedata.fb_dist_tol = distol;
    return;
  }; //set_fb_distol

  void set_fb_gridsize(unsigned int gridsize){
    guidedata.fb_coarse_grid = gridsize;
    return;
  }; //set_fb_gridsize

  void set_fb_cenbox(unsigned int cenbox){
    guidedata.fb_cen_box = cenbox;
    return;
  }; // set_fb_cenbox

  void set_fb_apbox(unsigned int apbox){
    guidedata.fb_ap_box = apbox;
    return;
  }; //set_fb_apbox

  void set_fb_offset(float offset){
    guidedata.fb_offset = offset;
    return;
  }; // set_fb_offset
  
  void set_fb_rn(float rn){
    guidedata.fb_rn = rn;
    return;
  }; // set_fb_rn

  void get_fb_vals(float *gain, uint16_t *satval,
		   float *thresh, int *distol,
		   unsigned int *gridsize, unsigned int *cenbox,
		   unsigned int *apbox, float *offset,
		   float *rn){
    *gain = (float) guidedata.fb_gain;
    *satval = guidedata.fb_cam_sat;
    *thresh = guidedata.fb_sigma_thresh;
    *distol = guidedata.fb_dist_tol;
    *gridsize = guidedata.fb_coarse_grid;
    *cenbox = guidedata.fb_cen_box;
    *apbox = guidedata.fb_ap_box;
    *offset = guidedata.fb_offset;
    *rn = guidedata.fb_rn;
    return;
  }; //get_fb_vals

  void reset_fb(){
    guidedata.fb_cam_sat=FB_CAM_SAT;
    guidedata.fb_coarse_grid=FB_COARSE_GRID;
    guidedata.fb_cen_box=FB_CEN_BOX;
    guidedata.fb_ap_box=FB_AP_BOX;
    guidedata.fb_sigma_thresh=FB_SIGMA_THRESH;
    guidedata.fb_dist_tol=FB_DIST_TOL;
    guidedata.fb_rn=FB_RN;
    guidedata.fb_offset=FB_OFFSET;
    guidedata.fb_gain=FB_GAIN;
    return;
  };

  void setMaxStars(unsigned char mx){
    unsigned char newmax;
    newmax = mx;
    if ( (unsigned int) mx < 1 ) {
      newmax = 1;
    }
    if ( (unsigned int) mx > MAXSTARS ){
      newmax = MAXSTARS;
    }
    guidedata.maxstars=newmax;
    return;
  };


  void get_guide_stats(float *dx, float *dy, float *dr, float *rmsdx, float *rmsdy, float *rmsdr, float *msx, float *msy){
    *dx=offval[0];
    *rmsdx=offrms[0];
    *dy=offval[1];
    *rmsdy=offrms[1];
    *dr=offval[2];
    *rmsdr=offrms[2];
    *msx=offval[3];
    *msy=offval[4];
    
      
    return;
  };
  #define OFFMAX 100
  void compile_guide_stats(float dx, float dy, float dr, float sx, float sy){
    // adding sx, sy to existing function -- Ignacio, Meghna
    // computes an average of recent guide stats


    static char initialized=0;
    static float mx[OFFMAX], my[OFFMAX], mr[OFFMAX], xsq[OFFMAX], ysq[OFFMAX], rsq[OFFMAX], msx[OFFMAX], msy[OFFMAX];
    static unsigned char index;
    static float xsum,ysum,rsum,xsqsum,ysqsum,rsqsum, sxsum, sysum;
    if ( ! initialized ) {
      for(int i=0; i< OFFMAX; i++)
	mx[i]=my[i]=mr[i]=xsq[i]=ysq[i]=rsq[i]=msx[i]=msy[i]=0.0;
      index=0;
      initialized=1;
    }; // not initialized...

    if( (!isnan(dx)) && (!isnan(dy)) && (!isnan(dr) ) && (!isnan(sx)) && (!isnan(sy) ) ) {
      
      xsum -= mx[index];
      mx[index]=dx;
      xsum += mx[index];
      
      ysum -= my[index];
      my[index]=dy;
      ysum += my[index];
      
      rsum -= mr[index];
      mr[index]=dr;
      rsum += mr[index];
      
      xsqsum -= xsq[index];
      xsq[index]=dx*dx;
      xsqsum += xsq[index];
      
      ysqsum -= ysq[index];
      ysq[index]=dy*dy;
      ysqsum += ysq[index];
      
      rsqsum -= rsq[index];
      rsq[index]=dr*dr;
      rsqsum += rsq[index];

      sxsum -= msx[index];
      msx[index]=sx;
      sxsum += msx[index];

      sysum -= msy[index];
      msy[index]=sy;
      sysum += msy[index];

      
      offval[0] = xsum/OFFMAX;
      offval[1] = ysum/OFFMAX;
      offval[2] = rsum/OFFMAX;
      offval[3] = sxsum/OFFMAX;
      offval[4] = sysum/OFFMAX;
      
      offrms[0] = sqrt(xsqsum/OFFMAX - offval[0]*offval[0]);
      if (isnan(offrms[0])) offrms[0]=-1.0;
      offrms[1] = sqrt(ysqsum/OFFMAX - offval[1]*offval[1]);
      if (isnan(offrms[1])) offrms[1]=-1.0;
      offrms[2] = sqrt(rsqsum/OFFMAX - offval[2]*offval[2]);
      if (isnan(offrms[2])) offrms[2]=-1.0;
      index = (index+1) % OFFMAX;
      
    };
    return;
  };
  
  //};


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
  

  void write_guide_data(guide_data_t *gdt){
    static guide_log_data_t *gldt=NULL;
    if (gldt == NULL) {
      gldt=(guide_log_data_t *)malloc(sizeof(guide_log_data_t));
    };
    (*gldt).fbmode=(*gdt).fbmode;
    memcpy(gldt->x, gdt->x, MAXSTARS*sizeof(float));
    memcpy(gldt->y, gdt->y, MAXSTARS*sizeof(float));
    memcpy(gldt->f, gdt->f, MAXSTARS*sizeof(float));
    memcpy(gldt->tx, gdt->tx, MAXSTARS*sizeof(float));
    memcpy(gldt->ty, gdt->ty, MAXSTARS*sizeof(float));
    memcpy(gldt->tf, gdt->tf, MAXSTARS*sizeof(float));
    (*gldt).dx=(*gdt).dx;
    (*gldt).dy=(*gdt).dy;
    (*gldt).drot=(*gdt).drot;
    (*gldt).framenumber=(*gdt).framenumber;
    memcpy(gldt->use, gdt->use, MAXSTARS);
    memcpy(gldt->valid, gdt->valid, MAXSTARS);
    
    fblog::writebin(gldt,sizeof(guide_log_data_t));

    return;
  }; // write_guide_data

  void write_guide_text_data(guide_data_t *gdt){
    
    fblog::logmsg("PTG %ld %lf\t%.2f %.2f %.2f %.2f\t %.2f %.2f %.2f %.2f\t%.2f %.2f %.2f %.2f\t %.2f %.2f %.3f\t%d, %d, %d, %d\t%d, %d, %d, %d",
		  (*gdt).framenumber,
		  get_time(),
		  (*gdt).tx[0], (*gdt).ty[0], 
		  (*gdt).x[0], (*gdt).y[0],
		  (*gdt).tx[1], (*gdt).ty[1], 
		  (*gdt).x[1], (*gdt).y[1],
		  (*gdt).tx[2], (*gdt).ty[2], 
		  (*gdt).x[2], (*gdt).y[2],
		  (*gdt).dx, (*gdt).dy, 
		  (*gdt).drot,
		  (*gdt).valid[0],
		  (*gdt).valid[1],
		  (*gdt).valid[2],
		  (*gdt).valid[3],
		  (*gdt).use[0],
		  (*gdt).use[1],
		  (*gdt).use[2],
		  (*gdt).use[3]);
    return;
  };





  void subtract_rownoise(uint16_t *buf){
    static uint16_t  *prow, *pend;
    static float sigma=20.0;
    static float sum,v0,dv;
    static float sigclip=1.0,offset=360.0,adjsigma;
    static int i,ctr,maxctr;
    static int deltaminusone=540;
    //    static clock_t start,end;

    // clear the mask
    //    start=clock();
    pend=buf+1280*1080L-(deltaminusone+1);
    prow=buf;
    
    adjsigma=sigclip*sigma;

    maxctr=1280-(deltaminusone+1);
    while (prow < pend){
      sum=0;
      ctr=0;
      for(i=0;i<maxctr;i++){
	v0=prow[i];
	dv=v0-prow[i+deltaminusone];
	if( (  ( dv>-adjsigma ) && dv<adjsigma)){
	  sum+=v0;
	  ctr++;
	}; // if abs sigma
      }; // for i
      sum=sum/ctr;
      for(i=0;i<1280;i++) prow[i]=floor((float)prow[i]-sum+offset);
	
      prow+=1280;
    };
    return;
  };
  
  // used in place of median
  void start_construct_min_image(unsigned char count){
    subtract=0;
    do_median=1;
    maxct_median=count;
    ct_median=0;
    memset(medbuff,255,1280*1080L*2);
    median_first = img::getFramenumber();
    return;
  }

  void construct_min_image(uint16_t *ptr){
    uint16_t *mptr, *pptr, *eptr;
    static int i;
    fitsfile *fptr;
    int status;
    long fpixel =1, naxis=2, nelements, naxes[2];
    pptr=ptr;
    mptr=&medbuff[0];
    eptr = pptr+1280*1080L;
    while(pptr < eptr){
      if( *mptr > *pptr ) *mptr=*pptr;
      mptr++;pptr++;
    }

    
    ct_median++;
    if(ct_median >= maxct_median){
      mptr=&medbuff[0];
      eptr = mptr+1280*1080L;
      for (i=0;i<1280*1080L;i++){
	medbuff[i]-=350;
      };
      status=0;
      naxes[0]=1280;
      naxes[1]=1080;
      nelements=1280L*1080;
      char filename[100];
      fits_report_error(stderr,status);
      sprintf(filename,"./aux/minstack%07ld.fits",median_first);
      fits_create_file(&fptr, filename, &status);
      fits_create_img(fptr,USHORT_IMG, naxis, naxes, &status);
      fits_update_key(fptr, TINT, "NIMAGES", &ct_median, "Number of images used to construct", &status);
      fits_write_date(fptr, &status);
      fits_write_img(fptr,TUSHORT,fpixel,nelements, (void*)&medbuff[0], &status);
      fits_close_file(fptr,&status);
      fits_report_error(stderr,status);
      
      do_median=0;
      subtract=3;
      ct_median=0;
    };
    
    
    return;
  };


  void load_median_image(){
    fitsfile *fitsfile;
    char fname[256];
    int bitpix;
    int status; 
    int naxis[2];
    long naxes[2];
    long fpixel[2]; //,lpixel[2];
    long inc[2];
    inc[0]=inc[1]=1;
    fpixel[0]=1;
    fpixel[1]=1;
    
    sprintf(fname,"/home/salo/FB/Fireball2/fbguider/lib/skytest_median.fits");
    status=0;
    ffiopn(&fitsfile,fname,0,&status);
    ffgipr(fitsfile,2,&bitpix,&naxis[0],&naxes[0],&status);
    ffgsv(fitsfile, TUSHORT, 
	  fpixel, naxes,
	  inc, 0 , &medbuff[0] ,NULL ,
	    &status);
    fits_close_file(fitsfile, &status);
    
    for(status=0; status<1280*1080L; status++)
      medbuff[status] -= 360;
    
    return;
  };
  

  void subtract_median(uint16_t *buf){
    static uint16_t *ebuf,*pbuf,*mbuf;
    ebuf=buf+1280*1080L;
    mbuf=&medbuff[0];
    pbuf=buf;
    while(pbuf<ebuf)
      *(pbuf++)-=*(mbuf++);
   
    return;
  };

  void set_subtract(unsigned char s){
    if (s > 3){
      subtract=3;
    } else { 
      subtract = (int)s;
    }
    return;
  }; //set_subtract
  
  unsigned char get_subtract(){
    return (unsigned char) subtract;
  }; // get_subtract

  void set_centroiding_algorithm(unsigned char s){
    if (s > 3){
      algorithm=3;
    } else { 
      algorithm=s;
    };
    return;
  }; //set_centroiding_algorithm

  unsigned char get_centroiding_algorithm(){
    return algorithm;
  }; //get_centroiding_algorithm

  void set_image_tag(unsigned short s){
    static char msg[32];
    if (s > 0){
      imagetag=s;
    } else {
      imagetag++;
    };
    
    fblog::logmsg("TAG %d",imagetag);
    sprintf(msg,"TAG %d",imagetag);
    ground::send_text(msg);
    save_tagnumber();
    return;
  }; //set_image_tag

  unsigned short get_image_tag(){
    return  imagetag;
  }; // get_image_tag

  void set_image_save_period(unsigned short sp){
    saveperiod=sp;
    return;
  }; // set_image_save_period;

  unsigned short get_image_save_period(){
    return saveperiod;
  }


  void get_profiles(unsigned char starid, void *spt){
    memcpy(spt,(void *)&curprofile[starid],sizeof(star_profile_t));
    return;
  };

  void set_profiles(unsigned char starid, unsigned char val, unsigned char used, float *xprofile, float *yprofile, float *xfit, float *yfit,long frameno){
    static int i;
    curprofile[starid].framenumber=frameno;
    curprofile[starid].starmask=starid+16*val+32*used;
    for(i=0;i<=2*IMG_BOXSIZE;i++){
      curprofile[starid].xprofile[i]=(unsigned short) floor(xprofile[i]);
      curprofile[starid].yprofile[i]=(unsigned short) floor(yprofile[i]);      
    };
    memcpy(&curprofile[starid].xfit[0],xfit,16);
    memcpy(&curprofile[starid].yfit[0],yfit,16);
    motors::get_motor_positions(curprofile[starid].stagepos);
    curprofile[starid].cx=boxes.cx[starid];
    curprofile[starid].cy=boxes.cy[starid];
    curprofile[starid].cut=boxes.star_cut;
    return;
  };
  
  void find_minmax(float *xprof, float *yprof, int size, int smooth, float *xmin, int *xminidx, float *xmax, int *xmaxidx, float *ymin, int *yminidx, float *ymax, int *ymaxidx){
    static int idx;
    static float xmaxval,totxmaxval;
    static float xminval,totxminval;
    static float ymaxval,totymaxval;
    static float yminval,totyminval;
    static float xtot,ytot;
    static int xmnidx,ymnidx,xmxidx, ymxidx;
    
    xmaxval=0;ymaxval=0;
    xminval=1e8; yminval=1e8;
    xtot=0; ytot=0;
    for(idx=0;idx<smooth;idx++){
      xtot+=xprof[idx];
      ytot+=yprof[idx];
    }; //foridx
    
    xmaxval=xprof[smooth-1];
    xminval=xprof[smooth-1];
    ymaxval=yprof[smooth-1];
    yminval=yprof[smooth-1];
    totxminval=xtot;
    totxmaxval=xtot;
    totyminval=ytot;
    totymaxval=ytot;
    xmnidx=smooth-1;
    xmxidx=smooth-1;
    ymnidx=smooth-1;
    ymxidx=smooth-1;

    
    for(idx=smooth; idx<size-smooth-1; idx++){
      xtot+=xprof[idx]-xprof[idx-smooth];
      ytot+=yprof[idx]-yprof[idx-smooth];
      //      printf("%f %f %f %f %f %f\n",xtot,xmaxval,totxmaxval,ytot,ymaxval,totymaxval);
      if(xtot> totxmaxval){
	xmaxval=xprof[idx];
	xmxidx=idx;
	totxmaxval=xtot;
      };
      if(xtot<totxminval){
	xminval=xprof[idx];
	xmnidx=idx;	
	totxminval=xtot;
      };

      if(ytot> totymaxval){
	ymaxval=yprof[idx];
	ymxidx=idx;
	totymaxval=ytot;;
      };
      if(ytot<totyminval){
	yminval=yprof[idx];
	ymnidx=idx;
	totyminval=ytot;
      };


    }; //foridx
    
    *xmin=xminval;
    *ymin=yminval;
    *xmax=xmaxval;
    *ymax=ymaxval;
    *xmaxidx=xmxidx;
    *xminidx=xmnidx;
    *ymaxidx=ymxidx;
    *yminidx=ymnidx;
    return;
  }; //find_minmax

  void refine_targets(){
    static int idx,finish;
    finish=0;
    for(idx=0;idx<MAXSTARS;idx++){
      if(guidedata.valid[idx]){
	cx_history[idx]+=guidedata.x[idx];
	cy_history[idx]+=guidedata.y[idx];
	ct_history[idx]++;
	if (ct_history[idx] >= ct_history_max) finish++;
      };
    };
    if (finish){
      for(idx=0;idx<MAXSTARS;idx++){
	if(ct_history[idx]){
	  cx_history[idx] /= ct_history[idx];
	  cy_history[idx] /= ct_history[idx];
	  guidedata.tx[idx]=cx_history[idx];
	  guidedata.ty[idx]=cy_history[idx];
	};

      };
      sprintf(msg,"Refinement done.");
      ground::send_text(msg);
      do_history=0;
    };
    return;
  };

  void start_refine_targets(unsigned char count){
    memset(cx_history,0,MAXSTARS*4);
    memset(cy_history,0,MAXSTARS*4);
    memset(ct_history,0,MAXSTARS*4);
    do_history=1;
    ct_history_max=count;
    return;
  };

int load_tagnumber(){
  static FILE *fp;
  static unsigned short int fn;
  int res;
  fp = NULL;
  fp = fopen(TAGNUMBER_FILENAME,"r");
  if(fp != NULL){
    res = fscanf(fp,"%hu\n",&fn);
    if(res != 1){
      imagetag = 0;
      fblog::logerr("Failed to read in tagnumber. Setting 0.");
    } else {
      imagetag = fn;
      fblog::logmsg("Loaded tagnumber: %d",imagetag);
    };
    fclose(fp);

  } else {
    imagetag = 0;
    fblog::logmsg("Assigned ftagnumber: 0");
  }; 
      return 0;
}; //fbcamera::load_tagnumber

int save_tagnumber(){
  static FILE *fp;
  fp = fopen(TAGNUMBER_FILENAME,"w");
  fprintf(fp,"%d\n",imagetag);
  fclose(fp);
  return 0;
}; //save_tagnumber


}; // namespace img

