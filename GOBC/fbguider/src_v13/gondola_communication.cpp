#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <math.h>

#include "gondola_communication.h"
#include "fblog.h"
#include "timing.h"
#include "parameters.h"



namespace gondola{
  unsigned short outbound_counter=0;
  unsigned short inbound_counter=0;
  unsigned short us;
  
  double ra,dec,rot;
  
  // structures to populate -- lv is "last valid"
  gondola_status_t gondola_status; 
  gondola_status_t guider_status={ 
    0, // count
    GG_STATUS_LENGTH, // length 
    GG_STATUS_ID,  //id
    0, // status
    0 // CRC
  }; 

  gondola_DTU_data_t gondola_DTU_data;

  gondola_ground_boresight_t gondola_ground_boresight, gondola_ground_boresight_lv;


  guider_boresight_t guider_boresight={
    0, //count
    GG_GUIDER_BORESIGHT_LENGTH, //length
    GG_GUIDER_BORESIGHT_ID, //ID
    0, // sec
    0, // nsec
    0, //alpha
    0, //delta
    0, //rot
    0, //valid
    0 // CRC
  };


  guider_rotation_error_t guider_rotation_error={ 
    0,
    GG_GUIDER_ROTERROR_LENGTH,
    GG_GUIDER_ROTERROR_ID,
    0, // sec
    0, // nsec
    0, // rot
    0, // valid
    0 //CRC
  };

  
  
  // returns -1 if there is a mismatch
  unsigned char gondola_counter_received(unsigned short ctr){
    if(ctr != inbound_counter){
      inbound_counter = ctr;
      return -1;
    } else { 
      inbound_counter++;
      return 0;
    };
  }; // gondola_counter_received

  inline void gondola_time(int32_t *sec, int32_t *nsec){
    message_time(sec, nsec);
    return;
  };
  

  // STATUS MESSAGE HANDLERS 0x00
  void update_guider_status(unsigned char status){
    guider_status.status = status;
  }; // make_gondola_status

  void get_gondola_status(unsigned char *status){
    *status = gondola_status.status;
  }; // get_gondola_status

  void send_guider_status(){
    guider_status.counter = GONDOLA_COUNTER();
    guider_status.crc = COMPUTE_GONDOLA_CRC(guider_status);
    WRITE_TO_GONDOLA(guider_status);
    return;
  }; // send_guider_status();


  // DTU MESSAGE HANDLERS 0x01
  // this message will processed later...
 
  // GONDOLA_GROUND_BORESIGHT 0x02
  char get_gondola_boresight(float *az, float *el, float *rot){
    if(gondola_ground_boresight.valid){
      *az = gondola_ground_boresight.azimuth;
      *el = gondola_ground_boresight.elevation;
      *rot = gondola_ground_boresight.rotation;
      fblog::logmsg("GBORE 1 %.3lf %.4lf %.4lf %.4lf",gondola_ground_boresight.sec+gondola_ground_boresight.nsec/1.0e9,*az,*el,*rot);
      return 1;
    } else {
      fblog::logmsg("GBORE 0");
      return 0;
    };
  }; // get_gondola_boresight

  
  // GUIDER_BORESIGHT 0x03
  void update_guider_boresight(float alpha, float delta,\
			  float rotation, unsigned char valid){
    gondola_time(&guider_boresight.sec, &guider_boresight.nsec);
    guider_boresight.alpha = alpha;
    guider_boresight.delta = delta;
    guider_boresight.rotation = rotation;
    guider_boresight.valid = valid;   
    DEBUGPRINT("Guider_Boresight %d %d %f %f %f %d",guider_boresight.sec,guider_boresight.nsec,guider_boresight.alpha,guider_boresight.delta,guider_boresight.rotation,guider_boresight.valid);
    return;
  }; //update_guider_boresight

  void send_guider_boresight(){
    guider_boresight.counter = GONDOLA_COUNTER();
    guider_boresight.crc = COMPUTE_GONDOLA_CRC(guider_boresight);
    WRITE_TO_GONDOLA(guider_boresight);
  }; // send_guider_boresight
  
  // GUIDER ROTATION ERROR 0x04
#define ROTAVG 33
  void update_guider_rotation_error(float roterror, unsigned char valid){
    static int idx=0;
    static int ctr=0;
    static float rotarr[ROTAVG];
    static float last=0,total=0;
    if (valid){
      last=rotarr[idx];
      total-=last;
      rotarr[idx]=roterror;
      total+=roterror;
      ctr++;
      idx++;
      idx= idx % ROTAVG;
      if (ctr > ROTAVG){
	guider_rotation_error.roterror = total/ROTAVG;
      } else { 
	guider_rotation_error.roterror = total/ctr;
      };
      guider_rotation_error.roterror = roterror;
    } else { 
      guider_rotation_error.roterror = 0.0;
    };
    gondola_time(&guider_rotation_error.sec, &guider_rotation_error.nsec);
    guider_rotation_error.valid = valid;
    return;
  }; // update_guider_rotation_error
  
  void send_guider_rotation_error(){
    guider_rotation_error.counter = GONDOLA_COUNTER();
    guider_rotation_error.crc = COMPUTE_GONDOLA_CRC(guider_rotation_error);
    WRITE_TO_GONDOLA(guider_rotation_error);
  }; // send_guider_rotation_error


  void process_gondola_messages(unsigned char *buf, int size){
    static unsigned char * mid; //ok;
    static short *crc;
    static unsigned short *length; //, *count;
    //ok =0;
    if (size >= MIN_MESSAGE_SIZE){
      mid = (buf+4);
      length = (unsigned short *) (buf+2);
      // count = (unsigned short *) (buf);
      crc = (short *) (buf+size-2);
      if(size==(*length)+7){
	switch(*mid){
	case GG_STATUS_ID:
	  if( (*length) == GG_STATUS_LENGTH)
	    if( (*crc) == compute_gondola_crc(buf,size-2)){
	      memcpy((char*)&gondola_status,buf,size);
	      //ok=1;
	      //	      DEBUGPRINT("Received STATUS_ID");
	    };
	  break;
	case GG_DTU_DATA_ID:
	  if( (*length) == GG_DTU_DATA_LENGTH)
	    if( (*crc) == compute_gondola_crc(buf,size-2)){
	      memcpy((char*)&gondola_DTU_data,buf,sizeof(gondola_DTU_data_t));
	      gondola::extract_quaternions(gondola_DTU_data.data);
	      //ok=1;
	      //DEBUGPRINT("Received DTU_DATA_ID");
	      //printf("Received DTU_DATA_ID");
	    };
	  break;
	case GG_GONDOLA_BORESIGHT_ID:
	  if( (*length) == GG_GONDOLA_BORESIGHT_LENGTH )
	    if( (*crc) == compute_gondola_crc(buf,size-2) ){
	      memcpy((char*)&gondola_ground_boresight,buf,size);
	      //	      DEBUGPRINT("Received Message Gondola_Boresight_ID %d %d %f %f %f %d",gondola_ground_boresight.sec,gondola_ground_boresight.nsec,gondola_ground_boresight.azimuth,gondola_ground_boresight.elevation,gondola_ground_boresight.rotation,gondola_ground_boresight.valid);
	      // ok=1;
	    };
	  break;
	  // Should not receive those next two
	  //      case GG_GUIDER_BORESIGHT_ID:
	  //	break;
	  //      case GG_GUIDER_ROTERROR_ID:
	  //	break;
	default:
	  // not a valid message ID!
	  // maybe do something in the future, but not now.
	  break;
	};//switch(*mid)
      }; // size == length+7
      
    } else { 
      // the message received is not minimum size
    };    
    //    if(ok){
    //   if(!gondola_counter_received(*count)) {
	//	fblog::logerr("Skipped some gondola messages.");
    //	        fblog::logerr("Skipped some gondola messages. %d %d",*count,gondola_counter_received(*count));} 
    //   else {
    //	        fblog::logerr("Received a gondola messages. %d %d",*count,gondola_counter_received(*count));} 
    //   };
    return;
  }; // process_gondola_messages


  /*----------------------------------------------------------------------------*/
  /* CaclulateCheckSum                                                    */
  /*----------------------------------------------------------------------------*/
  /*! Effectue le calcul du checksum sur le buffer d'octets en entree. Cette
   *  methode a ete directement recupere du protocole FivCo.
   *
   *\param       (input) unsigned char * ByteTab : Buffer de donnees.
   *\param       (input) int Size : Nombre d'octets dans le buffer.
   *\retval      Checksum calcule sur le tableau de donnees.
   *\warning Aucun.   
   */
  /*----------------------------------------------------------------------------*/
  short compute_gondola_crc( unsigned char * ByteTab, int Size )
  {
    // This function return the checksum calculated
    static unsigned int Sum;
    static bool AddHighByte;
    static unsigned int ChecksumCalculated;

    Sum=0;
    AddHighByte = true;
    ChecksumCalculated = 0;

    for(int i=0;i<Size;i++)
      {
	if(AddHighByte)
	  {
	    Sum+=((ByteTab[i])<<8)^0xFF00;
	    AddHighByte=false;
	  }
	else
	  {
	    Sum+=(ByteTab[i])^0x00FF;
	    AddHighByte=true;
	  }
      }
    if (AddHighByte==false)
      Sum+= 0xFF;
    
    ChecksumCalculated = ((Sum>>16)&0xFFFF)+(Sum&0xFFFF);
    ChecksumCalculated = ((ChecksumCalculated>>16)&0xFFFF)+(ChecksumCalculated&0xFFFF);


    
    return ( short ) ChecksumCalculated;
  }; // compute_gondola_crc();

  void extract_quaternions(unsigned char *data){
    static unsigned char *cptr,*qptr;
    static long dq1,dq2,dq3,dq4;
    static double dec,ra,roll,q[4];
    static double q12, q13, q24,q34;
    static int i;



    cptr = (unsigned char*)data+18;
    qptr=(unsigned char *)(&dq1);
    for(i=0;i<3;i++) qptr[3-i]=cptr[i];

    cptr = (unsigned char*)data+22;
    qptr=(unsigned char *)(&dq2);
    for(i=0;i<3;i++) qptr[3-i]=cptr[i];

    cptr = (unsigned char*)data+26;
    qptr=(unsigned char *)(&dq3);
    for(i=0;i<3;i++) qptr[3-i]=cptr[i];

    cptr = (unsigned char*)data+30;
    qptr=(unsigned char *)(&dq4);
    for(i=0;i<3;i++) qptr[3-i]=cptr[i];
 
      
    q[0]=dq1/2147483647.0;
    q[1]=dq2/2147483647.0;
    q[2]=dq3/2147483647.0;
    q[3]=dq4/2147483647.0;


    q[0]=0.770;
    q[1]=0.100;
    q[2]=0.565;
    q[3]=0.280;
    
    q12=q[0]*q[1];
    q13=q[0]*q[2];
    q24=q[1]*q[3];
    q34=q[2]*q[3];
    
    ra = atan2(q34-q12, q13+q24);
    if (ra < 0.0 ){
      ra = ra+2*M_PI;
    };
    
    dec=asin(q[3]*q[3]+q[0]*q[0]-q[1]*q[1]-q[2]*q[2]);
    roll=atan2(q34+q12, q13-q24);


    set_gondola_radec(ra*180.0/3.14159,dec*180.0/3.14159,roll*180.0/3.14159);

    //    printf("QQL: %ld %ld %ld %ld\n",dq1,dq2,dq3,dq4);
    //    printf("QQD: %.3lf %.3lf %.3lf %.3lf\n",q[0],q[1],q[2],q[3]);
    //printf("Att: %lf %lf %lf\n\n",ra,dec,roll);
    return;
  };

  void set_gondola_radec(double ira, double idec, double iroll){
    ra=ira;
    dec=idec;
    rot=iroll;
  }; 


  void get_gondola_radec(double *ora, double *odec, double *oroll){
    *ora = ra;
    *odec = dec;
    *oroll = rot;
  };

}; // namespace gondola
