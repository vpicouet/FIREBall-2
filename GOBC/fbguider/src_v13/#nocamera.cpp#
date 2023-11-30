#include "nocamera.h"
#include "parameters.h"

#include "fitsio.h"

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unistd.h>

//for loading fits images
#include "fitsio.h"

namespace nocamera{
  
  // container variables
  int width; // width of image
  int height; // height of image
  unsigned int exptime; // fake exposure time in ms
  char initialized;
  // uint16_t *imgbuf;
  uint16_t *imgbuf;
  uint16_t buff[1280*1080];
  uint16_t *framebuffers[20];

  // note, pretty much NO error checking. 
  void load_images(uint16_t **buffers){
    FILE *infile;
    fitsfile *fitsfile;
    int imnos[20],imno;
    int i,ct;
    int err;
    char fname[256];
    int bitpix;
    int naxis[2];
    long naxes[2];
    //    long pixread;
    int status; 
    long fpixel[2]; //,lpixel[2];
    long inc[2];
    //uint16_t lptr;


    const char *list="/home/salo/FB/Fireball2/fbguider/lib/fieldlist.dat";
    infile=fopen(list,"r");

    

    inc[0]=inc[1]=1;

    for(i=0; i<20; i++){
      err=fscanf(infile, "%d", &imnos[i]);
      if ( err == EOF) break;
      DEBUGPRINT("%d\n",imnos[i]);
    };

    fclose(infile);
    ct=i;

    fpixel[0]=1;
    fpixel[1]=1;
    // pixread=1280*1080L;
    for(i=0;i<20;i++){
      imno=i % ct;

      //sprintf(fname,"/home/salo/FB/Fireball2/fbguider/bin_v05/img_save/stack%06d.fits",imnos[imno]);
      sprintf(fname,"%s/stack%06d.fits",LIBIMAGEDIR,imnos[imno]);
      DEBUGPRINT("%d: %s\n",imno,fname);
      // get information from fits file. 
      status=0;
      ffiopn(&fitsfile,fname,0,&status);
      ffgipr(fitsfile,2,&bitpix,&naxis[0],&naxes[0],&status);
      DEBUGPRINT("%d\n",status);
      // load image from fits file

      //int  fits_read_2d_[byt, sht, usht, lng, ulng, int, flt, dbl] /
      //ffg2d[b,i,ui,j,uj,k,e,d]
      //(fitsfile *fptr, long group, DTYPE nulval, long dim1, long naxis1,
      //long naxis2, > DTYPE *array, int *anynul, int *status)

      //ffg2d(fitsfile, , 0, fpixel, 2, l

      //int fits_read_subset / ffgsv
      //(fitsfile *fptr, int  datatype, long *fpixel, long *lpixel, long *inc,
      //DTYPE *nulval, > DTYPE *array, int *anynul, int *status)    
      ffgsv(fitsfile, TUSHORT, 
	    fpixel, naxes,
	    inc, 0 , buff ,NULL ,
	    &status);
      DEBUGPRINT("%d\n",status);
      fits_close_file(fitsfile, &status);
      //      DEBUGPRINT("%d \n",status);


      memcpy(framebuffers[i],buff,1280*1080L*2);
  //      ffgpxv(fitsfile,TUSHORT, fpixel, pixread, buff ,&status);
      // transfer images to buffer. 
    };



    return;
  };

  int init(uint16_t **buffers,uint16_t w, uint16_t h){
    width = w;
    height = h;
    DEBUGPRINT("%d %d\n",w,h);

    int i;
    for (i=0; i<20; i++){
      framebuffers[i]= (uint16_t *)malloc(1280*1080L*sizeof(uint16_t));
      DEBUGPRINT("%x\n",framebuffers[i]);
    };
    load_images(buffers);

    return 0;
  };

  int close(){
    return 0;
  };

  void set_exptime(unsigned int expt){
    exptime = expt;
    return;
  };

  void getimage(uint16_t *buf, uint32_t expt){
    static int lbufidx=0;
    
    imgbuf = buff;
    exptime = expt;
    memcpy(buf,framebuffers[lbufidx],1280*1080L*2);
    lbufidx++;
    lbufidx = lbufidx % 20;
    usleep(expt*1000);
    return;
  };

}; // namespace nocamera
