#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#define WIDTH 1280
#define HEIGHT 1082

#define STARS 3
#define XSHIFT 0.0
#define YSHIFT 0.0
#define ROT 2.5

#define MULTIPLIER 500.0

int main(int argc, char *argv[]){
  int idx, jdx , sdx;
  
  float xshift, yshift, rot;
  float dist;
  float mult;

  float xstar[STARS];
  float ystar[STARS];
  float sstar[STARS];
  
  float tx, ty;

  unsigned short int image[HEIGHT][WIDTH];

  FILE *fp;
  char fname[255];

  xshift = XSHIFT;
  yshift = YSHIFT;
  rot = ROT;
  mult = MULTIPLIER;

  xstar[0]=640.;
  ystar[0]=540.;
  sstar[0]=6;

  xstar[1] = 640.;
  ystar[1] = 142.;
  sstar[1] = 6;

  xstar[2] = 200.;
  ystar[2] = 542.;
  sstar[2] = 6;

  for(idx=0;idx<STARS; idx++)
    printf("%d %f %f %f\n",idx,xstar[idx],ystar[idx],sstar[idx]);

  // apply rotation around center of chip
  for(idx =0;idx<STARS; idx++){
    tx = 0; ty=0;
    printf("\n\n%d %f %f\n",idx,xstar[idx],ystar[idx]);
    tx = (-WIDTH/2.+xstar[idx])*cos(rot/180.*3.14159) \
      +(-HEIGHT/2.+ystar[idx])*sin(rot/180.*3.14159) \
      +WIDTH/2.;
    ty = -(-WIDTH/2.+xstar[idx])*sin(rot/180.*3.14159) \
      +(-HEIGHT/2.+ystar[idx])*cos(rot/180.*3.14159) \
      +HEIGHT/2.;
    xstar[idx] = tx + xshift;
    ystar[idx] = ty + yshift;
    printf("%f\n",rot);
    printf("%d %f %f\n",idx,xstar[idx],ystar[idx]);
  }; //idx1



  // construct the image
  for(jdx =0; jdx<HEIGHT; jdx++)
    for(idx =0;idx<WIDTH; idx++){
      for(sdx = 0; sdx<STARS; sdx++){
	dist = pow(idx-xstar[sdx],2.0)+
	  pow(jdx-ystar[sdx],2.0);
	image[jdx][idx]+= mult*(sdx+1)*exp(-dist/(2*sstar[sdx]*sstar[sdx]));
      };//sdx
    };// idx
  
  if(argc> 1){
    sprintf(fname, "%s.arr", argv[1]);
  } else {
    sprintf(fname, "stars.arr");
  };

  fp = fopen(fname,"w");
  fwrite(image,2,WIDTH*HEIGHT,fp);
  fclose(fp);



  

  return 0;
};
