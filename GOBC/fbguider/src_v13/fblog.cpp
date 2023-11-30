/* Jan 2016
Matuszewski (matmat@caltech.edu)
FIREBALL 2 log functionality
Everything is wrapped in an fblog namespace, and it is probably a good idea to just leave it that way, rather than saying 'using namespace fblog'
*/

#include <cstdio>
#include <ctime>
#include <cstdlib>

// we will need to control where data gets written from so we don't
// scramble stuff  
#include <pthread.h>

#include <cstdarg>

// error handling, etc
#include <sys/errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include "parameters.h"
#include "fblog.h"

namespace fblog{
  char txt_log_on; // is the log being used?
  char bin_log_on; // is the bin log being used? 
  char img_log_on; // are we saving images? 
  
  double log_start_time=0.0; // when were the logs opened? 
  FILE *ptxtlog;
  FILE *perrlog;
  FILE *pbinlog;
  FILE *pimglog;

  char txt_log_filename[255]; // filename for the text log
  char bin_log_filename[255]; // filename for the binary log
  char img_log_filename[255]; // filename for the image log

  // mutex locks 
  pthread_mutex_t txt_log_mutex=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t bin_log_mutex=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t img_log_mutex=PTHREAD_MUTEX_INITIALIZER;
  
  // are the logs open? 
  char txt_log_open=0;
  char bin_log_open=0;
  char img_log_open=0;
 
  // date string
  char datestring[255];
  
  // message level character
  char msglvl[] = { 'M','W','E' } ;


  int logopen(){
    // variable declarations
    timeval tv;
    time_t tim=time(NULL);
    struct stat s;
    int interr;
    int ret;
    // set the saving flags
    txt_log_on = TXT_LOG_ON;
    bin_log_on = BIN_LOG_ON;
    img_log_on = IMG_SAVE_ON;

    // get the start time 
    gettimeofday(&tv, NULL);
    log_start_time = tv.tv_sec+tv.tv_usec/1000000.;


    // create directory if will be writing logs and needed
    if (txt_log_on || bin_log_on) { 
      interr = stat(DEFAULT_LOGDIR, &s);
      if (interr == -1) {
	if ( errno == ENOENT ) {
	  ret = system("mkdir -p -v "DEFAULT_LOGDIR);
	  if(ret){
	  } else {
	  }; //ret
	} else {
	  perror("stat");
	  // if we encounter an error, we will just print out the
	  // error messages
	  txt_log_on = FALSE;
	  bin_log_on = FALSE;
	};
      } else { 
	if(!S_ISDIR(s.st_mode)) {  // exists, but not  a directory
	  perror("Not a directory: "DEFAULT_LOGDIR);
	  // if we encounter an error, we will just print out the
	  // error messages
	  txt_log_on = FALSE;
	  bin_log_on = FALSE;
	};
      }; 
    }; // txt and binary log directory creation

    // create directory if will be writing images and needed
    if (img_log_on){
      interr = stat(DEFAULT_IMGDIR, &s);
      if (interr == -1) {
	if ( errno == ENOENT ) {
	  ret = system("mkdir -p -v "DEFAULT_IMGDIR);
	  if(ret){
	  } else {
	  }; // ret 
	} else {
	  perror("stat");
	  // if we encounter an error, we will not be saving images
	  img_log_on = FALSE;
	};
      } else { 
	if(!S_ISDIR(s.st_mode)) {  // exists, but not  a directory
	  perror("Not a directory: "DEFAULT_IMGDIR);
	  // if we encounter an error, we will not be saving images
	  img_log_on = FALSE;
	};
      }; // err == -1
    }; // img_log_on
    img_log_open = img_log_on;

    
    // make a date string for when the logs are getting opened.
    tim = time(NULL);
    
    strftime(datestring, 255, "%Y%m%d-%H%M%S\0",gmtime(&tim));

    // lock mutexes, to make sure nothing can access

    //text log first:
    if (txt_log_open) {
      logerr("Text log already open.");
    } else {
      if (txt_log_on) {
	// create filename
	sprintf(txt_log_filename,"%s%s.log",DEFAULT_LOGDIR,datestring);
	ptxtlog = NULL;
	ptxtlog = fopen(txt_log_filename,"w");
	// do not use a logfile if failed to open
	if (ptxtlog == NULL){
	  fprintf(stderr,"Failed to open logfile: %s",txt_log_filename);
	  fflush(stderr);
	  sprintf(txt_log_filename,"stdout-stderr");
	  ptxtlog = stdout;
	  perrlog = stderr;
	} else {
	  perrlog = ptxtlog;
	  txt_log_open = TRUE;
	};
      } else {
	// not txt_log_on
	ptxtlog = stdout;
	perrlog = stderr;
	sprintf(txt_log_filename,"stdout-stderr");
      }; // txt_log_on
    }; //txt_log_open
    
    logmsg("Text log file: %s",txt_log_filename);
      
    // binary log next
    if (bin_log_open){
      logerr("Binary log already open.");
    } else {
      if (bin_log_on) { 
	// create filename
	sprintf(bin_log_filename,"%s%s.bin",DEFAULT_LOGDIR,datestring);
	pbinlog = NULL;
	pbinlog = fopen(bin_log_filename,"w");
	// do not use the logfile if failed to open
	if (pbinlog == NULL){
	  logerr("Binary file failed to open: %s",bin_log_filename);
	  bin_log_on = FALSE;
	} else {
	  logmsg("Bin log file: %s",bin_log_filename);
	  bin_log_open = TRUE;
	}; // pbinlog != NULL
      }; // bin_log_on
    }; // bin_log_open

    
    return 0;
  };
  
  int logclose(){    
    // close the img logging
    if (img_log_open){
      img_log_open = 0;
    }; // img_log_open;
    
    // close the binary log, if exists

    // lock the binary mutex
    pthread_mutex_lock(&bin_log_mutex);
    if(bin_log_on && bin_log_open){
      logmsg("Closing binary log: %s",bin_log_filename);
      fflush(pbinlog);
      fclose(pbinlog);
      bin_log_open = FALSE;
    }; // bin_log_on && bin_log_open
    // unlock the binary mutex
    pthread_mutex_unlock(&bin_log_mutex);

    // close the text log, if exists
    if (txt_log_on && txt_log_open){
      logmsg("Closing text log: %s",txt_log_filename);
      // lock the mutex
      pthread_mutex_lock(&txt_log_mutex);
      fflush(ptxtlog);
      fclose(ptxtlog);
      // set the stream to standard
      ptxtlog=stdout;
      perrlog=stderr;
      // unlock the mutex
      pthread_mutex_unlock(&txt_log_mutex);
    }; // txt_log_on && txt_log_open
    return 0;
  };


  int logflush(){
    return 0;
  }; //logflush

  int writebin(void *str, int len ){
    // single write at a time
    pthread_mutex_lock(&bin_log_mutex);
    fwrite(str,len,1,pbinlog);
    // stop.
    pthread_mutex_unlock(&bin_log_mutex);
    return 0;
  };

  int _msg_(int level, const char *fname, const int line, const char *format, ...){
    static char vstr[512], ostr[512]; // container for messages
    static va_list args;
    static double tnow;
    static timeval tv;
    
    // lock the text log file write
    pthread_mutex_lock(&txt_log_mutex); 
    
    // only generate a message if we are logging, or this is an error
    if ( ((txt_log_on) && (level == 0 || level == 1))
	 || level == 2 ){
      gettimeofday(&tv,NULL); //
      tnow = tv.tv_sec + tv.tv_usec/1000000. - log_start_time;
      va_start (args, format); // start variable list parsing
      vsprintf(ostr, format, args);
      sprintf(vstr,"[%c][%9.3lf][%s:%d] %s",msglvl[level],tnow,fname,line,ostr);
      va_end(args); // end variable list parsing


      if(!txt_log_on){
	// if we are not logging, we still want error output.
	if (level == 2) fprintf(stderr,"%s\n",vstr);
      } else {
	if (level == 2) { 
	  // if an error, log to console and log
	  fprintf(perrlog, "%s\n",vstr);
	  if (txt_log_open) fprintf(stderr,"%s\n", vstr);
	} else {
	  // if warning or message, log to log only
	  fprintf(ptxtlog, "%s\n",vstr);
	}; // level == 2
	//	fflush(ptxtlog);   //Added this to handle logging delays---check if helps
      }; //txt_log_on      
    }; // log only if error or message and logging turned on.

    // unlock the text log file write
    pthread_mutex_unlock(&txt_log_mutex); 

    return 0;
  }; //_msg_

  // image and binary writing functions go here



}; //namespace fblog

