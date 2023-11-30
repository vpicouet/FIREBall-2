
#include "ringbuffer.h"


char&  RingBuffer::operator[](int i){
  return buffer[(this->start + i) % this->size];
};


int RingBuffer::Start(){
  return start;
};

int RingBuffer::End(){
  return end;
};

int RingBuffer::push(char *sourcebuff, int length){
  int i;
  if(length+used < size){
    for(i=0;i<length;i++){
      buffer[(this->end+i) % this->size] = sourcebuff[i];
    };
    
    used += length;
    end += length;
    end = end % size;
    

    return length;
  } else {
    
    return 0;
  };
};

int RingBuffer::pop(int length, char *targetbuffer){
  int i;
  if(length<=used){
  for(i=0;i<length;i++){
    targetbuffer[i] = buffer[(start+i) % size];
  };

  used -= length;
  start = (start + length) % size;
    return length;
  } else {
    return 0;
  };
};



bool RingBuffer::isEmpty(){
  if(used==0) return true;
  return false;
};

int RingBuffer::strip(int length){
  if(length<=used){
    start = (start + length) % size;
    used -= length;
    return used;
  } else {
    return 0;
  };
  

};

