// standard stuff
#include <cstdio>
#include <cstdlib>
#include <cstdint>

// guider stuff
#include "adio.h"
#include "fblog.h"
#include "parameters.h"

// avantech stuff
#include "biodaq_compatibility.h"
#include <bdaqctrl.h>

using namespace Automation::BDaq;

namespace adio{
  int32_t startPort = 0;
  int32_t portCount = 1;

  int32_t DOstartPort = 0;
  int32_t DOportCount = 1;

  int32_t challenStart = 1;
  int32_t channelCount = 1;

  int card_configured = 0;
  int card_on = 0;
  int card_ao_on = 0;
  int card_do_on = 0;
  int card_ai_on = 0;
  int card_di_on = 0;

  // the output value
  unsigned char do_output[1] = {0};
  unsigned char di_input[1] = {0};


  double ai_input[AI_COUNT];

  InstantAoCtrl *instantAoCtrl;
  InstantDoCtrl *instantDoCtrl;
  InstantDiCtrl *instantDiCtrl;
  InstantAiCtrl *instantAiCtrl;
  
  // voltage offsets -- may be needed sometime
  float volt_offset[2] = {0.00, 0.00};


  // functions to open and close the ports
  int open_ports(){
    ErrorCode ret;
    card_on = CARD_ON;
    card_ao_on = ANALOG_SIGNALS_ON;
    card_ai_on = ANALOG_IN_ON;
    card_do_on = DIGITAL_OUT_ON;
    card_di_on = DIGITAL_IN_ON;
    
    if (card_on ){
      DeviceInformation devInfo(deviceDescription);
      // ANALOG OUT
      if(card_ao_on){
	// open the AO
	instantAoCtrl = AdxInstantAoCtrlCreate();
	// identify the device
	ret = instantAoCtrl -> setSelectedDevice(devInfo);
	if (!BioFailed(ret)) { 
	  // should really check both return values below.
	  ret = instantAoCtrl->getChannels()->getItem(0).setValueRange(VRANGE);
	  ret = instantAoCtrl->getChannels()->getItem(1).setValueRange(VRANGE);
	  card_ao_on = 1;
	  // zero both
	  analog_out_both(0.0, 0.0);
	  fblog::logmsg("AO Initialized.");
	} else {
	  card_ao_on = 0;
	  fblog::logerr("AO Failed to initialize");
	} // biofailed(ret) AO
      } else {
	fblog::logmsg("AO NOT Initialized");
      }; // if card_ao_on

      // DIGITAL OUT
      if(card_do_on){
	// open the DO
	instantDoCtrl = AdxInstantDoCtrlCreate();      
	ret = instantDoCtrl->setSelectedDevice(devInfo);
	if(!BioFailed(ret)) {
	  ICollection<PortDirection>* portDirection = instantDoCtrl->getPortDirection();
	  DioPortDir dir = Output ;
	  // we are using only one of the two 8 bit ports for output
	  portDirection->getItem(DIGITAL_INPORT).setDirection(dir) ;
	  portDirection->getItem(DIGITAL_OUTPORT).setDirection(dir) ;
	  do_output[0] = 0;
	  card_do_on=1;
	  ret = (ErrorCode) digital_all_off();
	  instantDoCtrl->Write(DIGITAL_INPORT,1,0);
	  if(BioFailed(ret)) {
	    card_do_on = 0;
	    fblog::logerr("DO Failed to initialize");
	  } else {
	    fblog::logmsg("DO Initialized");
	  };//digital_all_off failed...
	} else {
	  card_do_on = 0;
	  fblog::logerr("DO Failed to initialize");
	};
      } else { 
	card_do_on = 0;
	fblog::logmsg("DO NOT Initialized");
      };// if card_do_on
      
      // DIGITAL IN
      if(card_di_on){
	instantDiCtrl = AdxInstantDiCtrlCreate();
	ret = instantDiCtrl->setSelectedDevice(devInfo);
	if(!BioFailed(ret)) {
	  ICollection<PortDirection>* portDirection = instantDiCtrl->getPortDirection();
	  DioPortDir dir = Input;
	  portDirection->getItem(DIGITAL_INPORT).setDirection(dir);
	  fblog::logmsg("DI Initialized.");
	} else { 
	  card_di_on = 0;
	  fblog::logerr("DI NOT Initialized");
	}; // !BioFailed
      }; // card_di_on

      //ANALOG IN
      if(card_ai_on){
	instantAiCtrl = AdxInstantAiCtrlCreate();
	ret = instantAiCtrl->setSelectedDevice(devInfo);
	// should set the voltage here!
	if(!BioFailed(ret)) {
	  card_ai_on = 1;
	  fblog::logmsg("AI Initialized");
	} else { 
	  card_ai_on = 0;
	  fblog::logerr("AI Not initialized.");
	};
      };// card_ai_on
    } else { 
      
      card_ao_on = 0;
      card_do_on = 0;
      card_ai_on = 0;
      card_di_on = 0;
      card_on = 0;
      fblog::logmsg("AO NOT Initialized");
      fblog::logmsg("DO NOT Initialized");
      fblog::logmsg("AI NOT Initialized");
      fblog::logmsg("DI NOT Initialized");
      
    }; // card_on 
    return 0;
  }; // open ports
  
  
  int close_ports(){
    if (card_on){
      if(card_ao_on){
	instantAoCtrl->Dispose();
	card_ao_on = 0;
	fblog::logmsg("AO Closed");
      }; // ao_on
      if(card_do_on){
	instantDoCtrl->Dispose();
	card_do_on = 0;
	fblog::logmsg("DO Closed");
      }; // do_on
      if(card_di_on){
	instantDiCtrl->Dispose();
	card_di_on = 0;
	fblog::logmsg("DI Closed");
      }; // di_on

      if(card_ai_on){
	instantAiCtrl->Dispose();
	card_ai_on = 0;
	fblog::logmsg("AI Closed");
      }; // ai_on

    } else { 
      // nothing to do if nothing is open...
    }; // card_on

    return 0;
  };

  // analog output functions
  int analog_out_both(double out0, double out1){
    static double volts[2];
    
    if(card_ao_on){
      volts[0] = out0+volt_offset[0];
      volts[1] = out1+volt_offset[1];
      // cap the output, though the card probably does that anyway
      if(volts[0]< MINV) volts[0] = MINV;
      if(volts[0]> MAXV) volts[0] = MAXV;
      if(volts[1]< MINV) volts[1] = MINV;
      if(volts[1]> MAXV) volts[1] = MAXV;
      // output
      instantAoCtrl->Write(0,2,volts);
    } else { 
      // dangerous to put anything here -- would
      // run LOTS of times.
      return -1;
    };
    return 0;
  }; // analog_out_both

  int analog_out(analog_channel_t chan, double out){
    static double volt;
    if(card_ao_on){
      volt = out+volt_offset[chan];
      if (volt < MINV) volt = MINV;
      if (volt > MAXV) volt = MAXV;
      instantAoCtrl->Write((int)chan,1,&volt);
    } else {
      return -1;
    };
    return 0;
  }; // analog_out

  // digital output
  int digital_out(digital_channel_t chan, int out){
    if(card_do_on){
      if(out){ 
	return digital_on(chan);
      } else {
	return digital_off(chan);
      }; 
    };     
    return 0;
  }; // digital_out


  int digital_on(unsigned short chan){
    ErrorCode ret;
    if (card_do_on){
      do_output[0] |= (chan);
      ret=instantDoCtrl->Write(DIGITAL_OUTPORT,1,do_output);
      if(!BioFailed(ret)){
	fblog::logmsg("DO Channel %d ON",chan);
      } else { 
	fblog::logerr("DO Channel %d ON failed [%d]",chan,ret);
      };
      return ret;
    } else {
      return 0;
    };
    return 0;
  }; // digital_on

  int digital_off(unsigned short chan){
    ErrorCode ret;
    if (card_do_on){
      do_output[0] &= ~(chan);
      ret=instantDoCtrl->Write(DIGITAL_OUTPORT,1,do_output);
      if(!BioFailed(ret)){
	fblog::logmsg("DO Channel %d OFF",chan);
      } else { 
	fblog::logerr("DO Channel %d OFF failed",chan);
      };
      return ret;
    } else { 
      return 0;
    };
    return 0;
  }; // digital_off

  int digital_all_off(){
    ErrorCode ret;
    if (card_do_on){
      do_output[0] = 0;
      ret=instantDoCtrl->Write(DIGITAL_OUTPORT,1,do_output);
      if(!BioFailed(ret)){
	fblog::logmsg("All DO off");
      } else { 
	fblog::logerr("All DO off failed [%d]",ret);
      };
      return ret;
    } else { 
      return 0;
    };
    
    return 0;
  }; // digital_all_off


  // digital input
  int digital_single_in(digital_channel_t chan, unsigned char *in){
    static ErrorCode ret;
    ret = (ErrorCode) 0;

    if(card_di_on){
      ret = instantDiCtrl->Read(DIGITAL_INPORT,1,di_input);
      *in = chan & di_input[0];
    } else {
      return -1;
    };
    return ret;
  }; // digital_in

  int digital_in(unsigned char *in){
    static ErrorCode ret;
    ret = (ErrorCode) 0;
    
    if(card_di_on){
      ret = instantDiCtrl->Read(DIGITAL_INPORT,1,di_input);
      //DEBUGLINE;
      //printf("Digital in: %d %x \n",di_input[0],di_input[0]);
      // printf("----------------------------\n");
      *in = di_input[0];
    } else {
      return -1;
    };
    return ret;
  }; // digital_in

  int update_analog_in(){
    static ErrorCode ret;
    ret = (ErrorCode) 0;

    if(card_ai_on){
      ret = instantAiCtrl->Read(ANALOG_IN,AI_COUNT,ai_input);
      //DEBUGLINE;
      
      //      for(i=0;i<AI_COUNT;i++)
      //          printf("Ch %d: %lf V, ",i,ai_input[i]);
      //      printf("\n");
      return 0;
    } else { 
      return -1;
    };
    return ret;
  };// update_analog_in;



  void retrieve_pt(float *temp,float *pressure){
    static float tnew, pnew;
    tnew = ai_input[1]*1000.0-273.15; // 
    // convert from kPa to mbar
    pnew = (250.*(ai_input[2]/ai_input[0]+0.04))*9.872;
    average_pt(&tnew, &pnew, temp, pressure);
    fblog::logmsg("PT: %.3f %.3f %.3f",ai_input[0],ai_input[1],ai_input[2]);
    return;
  };

  void average_pt(float *newt, float *newp, float *t, float *p){
    static float temps[PTBUFSIZE], pressures[PTBUFSIZE];
    static int idx=0,cnt=0,i;
    static float ttmp, ptmp;
    static int maxctr;
    
    temps[idx]=*newt;
    pressures[idx]=*newp;

    temps[idx]=*newt;
    pressures[idx]=*newp;
    
    cnt++;
    
    if(cnt>=PTBUFSIZE){
      maxctr=PTBUFSIZE;
    } else {
      maxctr=cnt;
    }
    
    ttmp=0;ptmp=0;

    for(i=0;i<maxctr;i++){
      ttmp+=temps[i];
      ptmp+=pressures[i];
    };
    
    *t=ttmp/maxctr;
    *p=ptmp/maxctr;

    idx++;
    idx=idx % PTBUFSIZE;
    return;
  };


}; // namespace adio
