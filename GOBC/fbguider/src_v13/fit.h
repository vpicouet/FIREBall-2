#ifndef _FIT_H_
#define _FIT_H_


namespace fit{

  // arr is the array to fit
  // par are the parameters passed 
  // chi is the chi squared returned
  // n is the size of arr
  // m is the number of parameters 

  void find_gauss_centroids(float *arr0, float *par0, float *chi0,
			    float *arr1, float *par1, float *chi1,
			    int n, int m);


  // p : fit parameters:
  // g(x) = p[3] + p[1] * exp ( -  ( x-p[0])^2/(2*p[2]))
  // y : array values
  // m : # of parameters
  // n : # of data points
  // data : aux array, actually passes the x axis values
  void fb_gaussian(float *p, float *y, int m, int n, void *data);
  

};

#endif 
/* _FIT_H_*/
