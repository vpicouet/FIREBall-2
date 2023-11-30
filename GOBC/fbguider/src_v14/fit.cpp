#include <math.h>
#include <float.h>


#include <time.h>

#include "levmar.h"

#include "fit.h"

#ifndef LM_DBL_PREC
#error Program assumes that levmar has been compiled with double precision, see LM_DBL_PREC!
#endif


namespace fit {
  // arr is the array to fit
  // par are the parameters passed 
  // chi is the chi squared returned
  // n is the size of arr
  // m is the number of parameters 

  void find_gauss_centroids(float *arr0, float *par0, float *chi0,
			    float *arr1, float *par1, float *chi1,
			    int n, int m){
    static int initialized=0,i;
    static float x[300];

    if (!initialized){
      for(i=0;i<300;i++) x[i]=i*1.0;
    };

    
    slevmar_dif(fb_gaussian,par0,arr0,m,n,500,NULL,NULL,NULL,NULL,(void*) x);
    slevmar_dif(fb_gaussian,par1,arr1,m,n,500,NULL,NULL,NULL,NULL,(void*) x);
    
    return;
  };


  // p : fit parameters:
  // g(x) = p[3] + p[1] * exp ( -  ( x-p[0])^2/(2*p[2]))
  // y : array values
  // m : # of parameters
  // n : # of data points
  // data : aux array, actually passes the x axis values
  void fb_gaussian(float *p, float *y, int m, int n, void *data){
    static int i;
    static float *x,p2s;


    x=(float*) data;
    p2s=p[2]*p[2]*2;
    for (i=0;i<n; i++)
      y[i]=(p[3]+p[0]*exp(-((x[i]-p[1])*(x[i]-p[1])/(p2s))));
  };

 };
