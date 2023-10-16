//ASSIGN PIN TO EACH BIT OF DATAWORD
int dw0 = 13;  //A3
int dw1 = 2;   //A5
int dw2 = 4;   //A7
int dw3 = 3;   //A9
int dw4 = 5;   //A11
int dw5 = 6;   //A13
int dw6 = 7;   //A15
int dw7 = 8;   //B2
int dw8 = 9;   //B4
int dw9 = 10;  //B6
int dw10 = 11; //B8 
int dw11 = 12; //B10
int dw12 = A5; //B12
int dw13 = A4; //B14
int dw14 = A3; //C1
int dw15 = A2; //C3

//STROBE SIGNALS
int sLO = A1; //C5
int sHI = A0; //C7

//ARRAYS
byte uplinkBuffer[200];
int  dataword[16]   = {dw0,dw1,dw2,dw3,dw4,dw5,dw6,dw7,dw8,dw9,dw10,dw11,dw12,dw13,dw14,dw15};
bool bits[16]       = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
byte startupMsg[11] = {0x7e, 0x00, 0x74, 0x63, 0x53, 0x74, 0x61, 0x72, 0x74, 0x0a, 0x7e};
byte errorMsg[11]   = {0x7e, 0x00, 0x74, 0x63, 0x45, 0x72, 0x72, 0x20, 0x30, 0x0a, 0x7e};

//SIMPLE VARIABLES
bool errflag = 0;   //Flag used to only raise one error per packet
bool dw_read = 1;   //Keep track of whether we have read a dataword in already
int  debug   = 0;   //Debug mode flag: 1=testing mode, 0=flight mode
int  tracker = 0;   //Keep track of position in buffer
int  int1,int2;     //Variables to store decimal values of dataword
byte byte1,byte2;   //Variables to store ASCII character values of dataword

void setup() {
  int pinType = INPUT_PULLUP;                           //Default mode is INPUT_PULLUP
  if(debug>2) pinType = INPUT;                          //Change to INPUT for debug mode
  for(int i=0; i<16; i++) pinMode(dataword[i],pinType); //Set dataword pin modes
  pinMode(sHI,pinType);                                 //Set strobe pin modes
  pinMode(sLO,pinType);
  Serial.begin(38400,SERIAL_8N1);                       //Serial communication at 38400 8N1 with MUX Rx line
  while(!Serial);                                       //Wait for serial to connect
  Serial.write(startupMsg,sizeof(startupMsg));          //Send startup message to DET PC.
}

void loop() {
  
  dw_read=0; //Reset DW flag 

  while( digitalRead(sHI)==HIGH && digitalRead(sLO)==LOW ) { //Wait until STROBEs pulse, otherwise do nothing
    
    if(!dw_read) { //Only execute if this DW not already read in

      for(int i=0;i<16;i++) bits[i]=digitalRead(dataword[i]); //Read the pin values into the bit array

      if(debug>1) displayBinary(); //Show binary values in debug mode
      
      int1 = bits[15]*128 + bits[14]*64 + bits[13]*32 + bits[12]*16 + bits[11]*8 + bits[10]*4 + bits[9]*2 + bits[8]*1; //Convert binary values to decimal values 
      int2 = bits[7]*128 + bits[6]*64 + bits[5]*32 + bits[4]*16 + bits[3]*8 + bits[2]*4 + bits[1]*2 + bits[0]*1;

      byte1 = (byte)int1; //Use decimal values to initialize bytes
      byte2 = (byte)int2;

      if( tracker==0 && byte1==0x7e && ( byte2==0x00 || byte2==0x01 || byte2==0x02 || byte2==0x03 ) ) { add(byte1); add(byte2); } // Initialize packet
      else if( tracker >=2 && byte1!=0x7e && byte2!=0x7e ) { add(byte1); add(byte2); } // Add data bytes to packet
      else if( tracker >=2 && byte1!=0x7e && byte2==0x7e ) { add(byte1); add(byte2); readout(); } // Read out if data + footer
      else if( tracker > 2 && byte1==0x7e && byte2==0x7e ) { add(byte1); readout(); } // Read out if footer + footer
      else signalError();
      
      dw_read = 1; //Flag that the dataword has been read in

    } // End if(!dw_read) 
  } // End while(strobe)  
} // End loop()

//Add bytes to buffer
void add(byte b) {
  uplinkBuffer[tracker]=b;   //Add first byte
  tracker+=1;                    //Move tracker along
}

//Read current packet out to serial out
void readout() {
  Serial.write(uplinkBuffer,tracker); //Real output
  tracker=0;                          //Reset tracker
}

//Function called when corrupt packet is detected.
void signalError() {
  tracker=0; //Set the tracker back to zero and await new packet
  Serial.write(errorMsg,sizeof(errorMsg)); //Send error message to DET PC
  errflag=1; //Mark that error has been raised
}

//Debug-mode function for showing binary to user
void displayBinary() {
  for(int i=15;i>=8;i--) Serial.print(bits[i]);
  Serial.print(" ");
  for(int i=7;i>=0;i--) Serial.print(bits[i]);
  Serial.println("");
}



