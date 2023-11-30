/* 
Logging mechanism for Fireball guider.

If logging is turned off, error output gets redirected to stderr,
while regular output is ignored.  
*/
#ifndef FBLOG_H_
#define FBLOG_H_

#define DEFAULT_LOGDIR "./log/"
#define DEFAULT_IMGDIR "./img/"



// define a couple of macros
#define logmsg(fmt, ...) _msg_(0, __FILE__,__LINE__, fmt, ## __VA_ARGS__)
#define logwrn(fmt, ...) _msg_(1, __FILE__,__LINE__, fmt, ## __VA_ARGS__)
#define logerr(fmt, ...) _msg_(2, __FILE__,__LINE__, fmt, ## __VA_ARGS__)


namespace fblog{
  // open, close, and flush the logs
  int logopen();
  int logclose();
  int logflush();
  
  // text log output. Both write to the same log, but in simulate
  // mode, only err is output to screen and regular text is ignored.
  int _msg_(int level, const char *fname, const int line, const char *format, ...);


  // various image output functions go here. 

  int writebin(void *str, int len);

  
}; //namespace fblog


#endif // FBLOG_H_
