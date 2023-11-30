#ifndef ADIO_H_
#define ADIO_H_

// 20160125 MM
// Based on Static DIAOv5 by JG


#define deviceDescription L"PCI-1706, BID#0"

#define ADIO_POLL 500

#define VRANGE V_Neg10To10
#define MINV -10
#define MAXV 10

#define DIGITAL_OUTPORT 1
#define DIGITAL_INPORT 0 

#define AI_COUNT 8
#define ANALOG_IN 0

#define PTBUFSIZE 50

// elevation and cross elevation analog out channels
typedef enum analog_channel_t{
  elevation=0,
  crosselev=1
} analog_channel_t;

// lamps and shutters digital io channels
typedef enum digital_channel_t{
  shutA=1,
  shutB=2,
  shutC=4,
  shutD=8,
  lamp1=16,
  lamp2=32
}digital_channel_t;

namespace adio{
  
  // functions to open and close the ports
  int open_ports();
  int close_ports();
  
  // analog output functions
  int analog_out_both(double out0, double out1);
  int analog_out(analog_channel_t chan, double out);
  
  // digital output  
  int digital_out(digital_channel_t chan, int out);
  //  int digital_on(digital_channel_t chan);
  //  int digital_off(digital_channel_t chan);
  int digital_on(unsigned short chan);
  int digital_off(unsigned short chan);
  int digital_all_off();

  // digital input
  int digital_single_in(digital_channel_t chan, unsigned char *in);
  int digital_in(unsigned char *in);

  // analog input
  int update_analog_in(); 
  void retrieve_pt(float *temp,float *pressure);
  void retrieve_pt_multi(float *temp,float *pressure,float *gobc_temp, float *gobc_pressure,
                    float *cal_temp, float *cal_pressure);


  // there is noise on these analog inputs, so we are going to try to
  // average a bunch...  
  void average_pt(float *newt, float *newp, float *t, float *p);
  void average_pt_multi(float *newt, float *newp, float *t, float *p,
            float *gobc_newt, float *gobc_newp, float *gobc_t, float *gobc_p,
            float *cal_newt, float *cal_newp, float *cal_t, float *cal_p);

}; // namespace adio

#endif // ADIO_H_
