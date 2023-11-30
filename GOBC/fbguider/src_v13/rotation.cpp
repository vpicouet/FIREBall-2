#include "rotation.h"

float rotation(float newrot){
  static float rotarr[AVGSIZE];
  static int ctr=0;
  static int i;
  static float sum;
  rotarr[ctr % AVGSIZE]=newrot;
  sum=0;
  for(i=0;i<AVGSIZE;i++)
    sum+=rotarr[i];
  return sum;
};
