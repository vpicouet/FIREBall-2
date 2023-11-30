#include "stacking.h"
#include "fblog.h"
#include "parameters.h"
#include "ground_communication.h"
#include "detector_communication.h"
#include "gondola_communication.h"
#include "fbcamera.h"

// fits access
#include "fitsio.h" 

// access to motor variables 
#include "motors.h"

// access to digital IO values
#include "adio.h"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
namespace stacking{

  void *stack_and_save_thread_worker(void *sst_in){
    stacking_structure_t sst;
    static float el,az,grot, fccdt;
    static float mgs_t, mgs_p, gobc_t, gobc_p, cal_t, cal_p;
    static short ccdt, camt, pwst;
    static unsigned short ush;
    static const char *version_string=THE_VERSION_STRING;
    fblog::logmsg("FITS Writing Started.");
    sst = *((stacking_structure_t*)sst_in);
    char msg[255];
    
    fitsfile *fptr;
    int status;
    int err, len;
    char ret;
    long fpixel = 1, naxis = 2, nelements, naxes[2];
    char cmd[255];
    pid_t childPID;
    char filebase[255],filename[255],filename_msg[255];
    double ra, dec, rot;
    float tmpflt;
    status = 0;
    naxes[0] = sst.width;
    naxes[1] = sst.height;
    nelements = naxes[0]*naxes[1];
    // Save to directory 'img_save' for routine save files if mode=-1
    sprintf(filename,"%s.fits",sst.filename);
    fblog::logmsg("Writing fitsfile: %s",filename);
    fits_create_file(&fptr, filename, &status);

    fits_create_img(fptr, FLOAT_IMG, naxis, naxes, &status);


    // keywords
    long exptime, first_framenumber, last_framenumber, count;
    float tmppos;
    float tx[MAXSTARS],ty[MAXSTARS];
    float x[MAXSTARS],y[MAXSTARS];
    unsigned char valid[MAXSTARS],use[MAXSTARS];
    float f[MAXSTARS],sx[MAXSTARS],sy[MAXSTARS];
    int i;

    img::get_target_pointers(tx,ty);
    img::get_star_pointers(x,y,f,sx,sy);
    img::get_star_usevalid(use,valid);

    exptime = sst.exptime;
    first_framenumber = sst.first_framenumber;
    last_framenumber = sst.last_framenumber;
    count = sst.count;

    ush=img::get_image_tag();
    fits_update_key(fptr, TSTRING, "VERSION",
		    (void*)version_string,"Guider source code version", &status);

    fits_update_key(fptr, TUSHORT, "IMGTAG",
		    &ush,
		    "Human image tag", &status);

    fits_update_key(fptr, TLONG, "EXPOSURE",
		    &exptime,
		    "Total exposure time (ms)", &status);

    fits_update_key(fptr, TLONG, "FRAMESTA", 
		    &first_framenumber, 
		    "First frame number", &status);

    fits_update_key(fptr, TLONG, "FRAMEEND",
		    &last_framenumber,
		    "Last frame number", &status);

    fits_update_key(fptr, TLONG, "IMGCOUNT",
		    &count,
		    "Image count", &status);
   


    // put motor position data in the headers.
    tmppos=motors::get_motorpos(ROTAXIS); // rotator
    if(isnan(tmppos)) tmppos=-999.0;

    fits_update_key(fptr,TFLOAT,"ROTENC",
		    &tmppos,
		    "Rotator encoder reading",&status);

    tmppos=motors::get_motorpos(LINAAXIS); // linA
    if(isnan(tmppos)) tmppos=-999.0;
    fits_update_key(fptr,TFLOAT,"LINAENC",
		    &tmppos,
		    "Linear axis A encoder reading",&status);

    tmppos=motors::get_motorpos(LINBAXIS); // linB
    if(isnan(tmppos)) tmppos=-999.0;
    fits_update_key(fptr,TFLOAT,"LINBENC",
		    &tmppos,
		    "Linear axis B encoder reading",&status);

    tmppos=motors::get_motorpos(LINCAXIS); // linC
    if(isnan(tmppos)) tmppos=-999.0;
    fits_update_key(fptr,TFLOAT,"LINCENC",
		    &tmppos,
		    "Linear axis C encoder reading",&status);

    gondola::get_gondola_radec(&ra, &dec, &rot);
    if(isnan(ra)){
      ra=999.0;
    };
    if(isnan(dec)){
      dec=999.0;
    };
    if(isnan(rot)){
      rot=999.0;
    };
    fits_update_key(fptr,TDOUBLE,"RA_DTU",
		    &ra,
		    "RA (DTU)", &status);
    fits_update_key(fptr,TDOUBLE,"DEC_DTU",
		    &dec,
		    "Dec (DTU)", &status);
    fits_update_key(fptr,TDOUBLE,"ROLL_DTU",
		    &rot,
		    "Roll (DTU)", &status);

    

    if( gondola::get_gondola_boresight(&az, &el, &grot) ){
      if(isnan(az)) az=999.0;
      fits_update_key(fptr,TFLOAT,"AZ",
		      &az,
		      "Gondola azimuth", &status);

      if(isnan(el)) el=999.0;
      fits_update_key(fptr,TFLOAT,"EL",
		      &el,
		      "Gondola elevation", &status);
      if(isnan(grot)) grot=999.0;
      fits_update_key(fptr,TFLOAT,"MROT",
		      &grot,
		      "Gondola mask rotation angle", &status);

      retrieve_temperatures(&ccdt, &camt, &pwst);
      fccdt=ccdt/10.0;
      
      if(isnan(fccdt))	fccdt=999.0;      
      fits_update_key(fptr,TFLOAT,"CCDTEMP",
		      &fccdt,
		      "Guider CCD temperature.", &status);

      fits_update_key(fptr,TSHORT,"CAMTEMP",
		      &camt,
		      "Guider camera temperature.", &status);

      fits_update_key(fptr,TSHORT,"PWSTEMP",
		      &pwst,
		      "Guider camera power supply temperature.", &status);
      
    	// adding more temp readouts to header

    	adio::retrieve_pt_multi(&mgs_t,&mgs_p,&gobc_t,&gobc_p,&cal_t,&cal_p);
    	if(isnan(mgs_t))	mgs_t=999.0;
      fits_update_key(fptr,TFLOAT,"TRTD",
		      &mgs_t,
		      "Camera enclosure RTD temperature (C)", &status);
      if(isnan(mgs_p))	mgs_p=999.0;
      fits_update_key(fptr,TFLOAT,"Pressure",
		      &mgs_p,
		      "Camera enclosure pressure (mbar)", &status);
      if(isnan(gobc_t))	gobc_t=999.0;
      fits_update_key(fptr,TFLOAT,"GOBC_T",
		      &gobc_t,
		      "GOBC enclosure temperature (C)", &status);
      if(isnan(gobc_p))	gobc_p=999.0;
      fits_update_key(fptr,TFLOAT,"GOBC_P",
		      &gobc_p,
		      "GOBC enclosure pressure (mbar)", &status);

      if(isnan(cal_t))	cal_t=999.0;
      fits_update_key(fptr,TFLOAT,"CAL_T",
		      &cal_t,
		      "Calibration box temperature (C)", &status);
      if(isnan(cal_p))	cal_p=999.0;
      fits_update_key(fptr,TFLOAT,"CAL_P",
		      &cal_p,
		      "Calibration  pressure (mbar)", &status);






      //adio::retrieve_pt(&el, &az);
      //if(isnan(el))	el=999.0;
      //fits_update_key(fptr,TFLOAT,"TRTD",
		  //    &el,
		  //    "Camera enclosure RTD temperature (C)", &status);
      //if(isnan(az))	az=999.0;
      //fits_update_key(fptr,TFLOAT,"Pressure",
		  //    &az,
		  //    "Camera enclosure pressure (mbar)", &status);


      for(i=0;i<MAXSTARS;i++){
	sprintf(msg,"VALID%1d",i);
	fits_update_key(fptr, TUSHORT, msg,
			&valid[i],
			"Valid", &status);

	sprintf(msg,"USE%1d",i);
	fits_update_key(fptr, TUSHORT, msg,
			&valid[i],
			"Use", &status);

	sprintf(msg,"TX%1d",i);
	tmpflt=tx[i];
	if (isnan(tmpflt)) tmpflt=-999.0;
	fits_update_key(fptr,TFLOAT,msg,
		      &tmpflt,
		      "Target X", &status);

	tmpflt=ty[i];
	if (isnan(tmpflt)) tmpflt=-999.0;
	sprintf(msg,"TY%1d",i);
	fits_update_key(fptr,TFLOAT,msg,
		      &tmpflt,
		      "Target Y", &status);

	tmpflt=x[i];
	if (isnan(tmpflt)) tmpflt=-999.0;
	sprintf(msg,"CX%1d",i);
	fits_update_key(fptr,TFLOAT,msg,
		      &tmpflt,
		      "Centroid X", &status);

	tmpflt=y[i];
	if (isnan(tmpflt)) tmpflt=-999.0;
	sprintf(msg,"CY%1d",i);
	fits_update_key(fptr,TFLOAT,msg,
		      &tmpflt,
		      "Centroid Y", &status);

	tmpflt=f[i];
	if (isnan(tmpflt)) tmpflt=-999.0;
	sprintf(msg,"FLUX%1d",i);
	fits_update_key(fptr,TFLOAT,msg,
		      &tmpflt,
		      "Flux", &status);

	tmpflt=sx[i];
	if (isnan(tmpflt)) tmpflt=-999.0;
	sprintf(msg,"SIGMAX%1d",i);
	fits_update_key(fptr,TFLOAT,msg,
		      &tmpflt,
		      "Sigma X", &status);

	tmpflt=sy[i];
	if (isnan(tmpflt)) tmpflt=-999.0;
	sprintf(msg,"SIGMAY%1d",i);
	fits_update_key(fptr,TFLOAT,msg,
		      &tmpflt,
		      "Sigma Y", &status);

      };


      // gondola information was valid.
    } else { 
      // gondola information was not valid...
    };


    // add a time tag for when the file was saved:
    fits_write_date(fptr,  &status);



    fits_write_img(fptr, TFLOAT, fpixel, nelements, (void *)sst.buf, &status);
    fits_close_file(fptr, &status);
    
    fits_report_error(stderr, status);


    fblog::logmsg("FTIS Writing Done.");
    sprintf(filename_msg,"FITS Write Done. %s.fits",sst.filename);
    //    ground::send_text("FITS Write Done.");
    ground::send_text(filename_msg);
    len=sprintf(filename_msg,"SAV=%s.fits",sst.filename);
    write_to_detector(filename_msg,len);
    // astrometry stuff goes here
    // sst.filename contains the filename that was saved.

    // construct the command line, execute it
    // make sure solution was found, read in the data
    // adapt the data...
    // char test[]="2.0";
    //sprintf(filebase,"/home/salo/FB/astrometry/examples/apod2");
    //sprintf(cmd,"./run_astro.sh %s.jpg 85.57 -3.5 2.0",filebase);

    //   sprintf(cmd,"./run_astro.sh %s 37 0.5 3.0",ra,dec, filename);
    sprintf(cmd,"./run_astro.sh %s %.3lf %.3lf 3.0", filename, ra, dec);
    sprintf(filebase,"%s.fb",sst.filename);
    if (sst.astrometry_mode>0){
      childPID = fork();
      if (childPID >=0) {
	if (childPID ==0 ) {
	  // child process
	  
	  ret = system(cmd);
	  DEBUGPRINT("ret = %d\n",ret);
	  _exit(ret);
	} else {
	  // parent process
	  wait(&status);
	  DEBUGPRINT("status = %d\n",status);
	  DEBUGPRINT("Done waiting!\n");
	  if ( status == 0 ){
	    FILE *fp;
	    fp=fopen(filebase,"r");
	    if (fp != NULL ) {
	      err=fscanf(fp,"%lf %lf %lf",&ra,&dec,&rot);
	      if (err){
		DEBUGPRINT("Error reading file.\n");
	      };
	      fclose(fp);
	      DEBUGPRINT("SOLVED!\n\tRA=%lf\n\tDec=%lf\n\tRot=%lf\n",ra,dec,rot);
	      fblog::logmsg("Field %s solved: RA=%lf Dec=%lf Rot=%lf",filebase,ra,dec,rot);
	      sprintf(cmd,"S: r=%lf d=%lf t=%lf",ra,dec,rot);
	      ground::send_text(cmd);
	    } else { 
	      ground::send_text((char *)"Solve failed.");	      
	    };
	  } else {
	    ground::send_text((char *)"Solve failed.");
	  }
	};
      } else {
	DEBUGPRINT("Fork Failed.\n");
      };
    };
  
    return NULL;
  }; // stack_and_save_thread_worker


}; // namespace stacking
