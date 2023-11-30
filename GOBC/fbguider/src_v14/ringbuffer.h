/********************************************************************/
/*
Class ringbuffer.

Messages from various serial sources arrive in bits. This collects them 
and provides a tool for weeding through them to check for message
validity



*/
/********************************************************************/

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_
#include <cstdlib>

class RingBuffer{
 public:
  char *buffer;
  unsigned short size;
  unsigned short start;
  unsigned short end;
  unsigned short used;


 public:
  RingBuffer(unsigned short bufsize){
    buffer = ( char *) malloc(bufsize);
    size = bufsize;
    start = 0;
    end = 0;
    used = 0;
  };

  RingBuffer(){
    buffer = ( char *) malloc(4096);
    size = 4096;
    start = 0;
    end = 0;
    used = 0;
  };

  ~RingBuffer(){
    free(buffer);
  };
  
  // lets the buffer be referenced in a round-about fashion. 
  char &operator[](int i);
  
  // push moves a buffer of size length pointed to by sourcebuff
  // and inserts it into the ring buffer
  int push(char *sourcebuff, int length);

  // pop extracts the specified object from the beginning of the buffer
  int pop(int length, char *destbuff);

  // removes the first length characters of the buffer;
  int strip(int length);

  // returns true if the buffer is empty. 
  bool isEmpty();

  //  returns the start and end index of the buffer;
  int Start();
  int End();
};


bool isMessageValid(RingBuffer &rb);


#endif// RINGBUFFER_H_
