/*
*/
  int D2Lamp = 6;           //Digital IO Pin 6 controls D2 Lamp
  int D2Shutter = 7;        //Digital IO Pin 7 controls D2 Shutter
  int ZnLamp = 8;           //Digital IO Pin 8 controls Zn Shutter
  int ZnShutter = 9;        //Digital IO Pin 9 controls Zn Lamp
  
  int incomingByte;         //Control data over Serial port from Python program

  int sensorPinP = A0;      //Analog input Pin 0 from Pressure Sensor (0-5VDC = 0-1023)
  int sensorPinT = A1;      //Analog input Pin 1 from Temperature Sensor (0-5VDC = 0-1023)

  float sensorValueP = 0;   //initialize Pressure Sensor data as float
  float sensorValueT = 0;   //initialize Temperature Sensor data as float


// the setup function runs once when you press reset or power the board
void setup() {
  
  // initialize serial communication:
  Serial.begin(9600);
  
  // initialize digital pins as output.
  pinMode(D2Lamp, OUTPUT);
  digitalWrite(D2Lamp, HIGH);  //A = ON, B = OFF
  
  pinMode(D2Shutter, OUTPUT);
  digitalWrite(D2Shutter, LOW); //C = ON, D = OFF
  
  pinMode(ZnLamp, OUTPUT);
  digitalWrite(ZnLamp, LOW);  //E = ON, F = OFF
  
  pinMode(ZnShutter, OUTPUT);
  digitalWrite(ZnShutter, LOW); //G = ON, H = OFF

}

// the loop function runs over and over again forever
void loop() {

  if (Serial.available() > 0) {
    incomingByte = Serial.read();

    if (incomingByte == 'A') {      //sending A to arduino turns D2 lamp on
      digitalWrite(D2Lamp, LOW);
      Serial.println("D2 Lamp ON");
    }
    if (incomingByte == 'B') {      //sending B to arduino turns D2 lamp off
      digitalWrite(D2Lamp, HIGH);
      Serial.println("D2 Lamp OFF");
    }
    if (incomingByte == 'C') {      //sending C to arduino opens D2 shutter
      digitalWrite(D2Shutter, HIGH);
      Serial.println("D2 Shutter OPEN");
    }
    if (incomingByte == 'D') {     //sending D to arduino closes D2 shutter
      digitalWrite(D2Shutter, LOW);
      Serial.println("D2 Shutter CLOSED");
    }
    if (incomingByte == 'E') {      //sending E to arduino turns Zn lamp on
      digitalWrite(ZnLamp, HIGH);
      Serial.println("Zn Lamp ON");
    }
    if (incomingByte == 'F') {      //sending F to arduino turns Zn lamp off
      digitalWrite(ZnLamp, LOW);
      Serial.println("Zn Lamp OFF");
    }
    if (incomingByte == 'G') {     //sending G to arduino opens Zn shutter
      digitalWrite(ZnShutter, HIGH);
      Serial.println("Zn Shutter OPEN");
    }
    if (incomingByte == 'H') {     //sending H to arduino closes Zn shutter
      digitalWrite(ZnShutter, LOW);
      Serial.println("Zn Shutter CLOSED");
    }
    if (incomingByte == 'P') {     //sending P to arduino requests current pressure in kPa
      sensorValueP = analogRead(sensorPinP) * (5.000 / 1023);       //Convert Pressure Sensor 0-1023 bits to 0-5VDC
      float pressure = (((sensorValueP/5.000) + 0.040) / 0.004);    //Contert Pressure Sensor 0-5VDC to kPa
      Serial.print(pressure);     //Output Pressure to Serial Port
      Serial.println(" kPa");
    }
    if (incomingByte == 'T') {     //sending T to arduino requests current temperature in Kelvin
      sensorValueT = analogRead(sensorPinT) * (5.000 / 1023);       //Convert Temperature Sensor 0-1023 bits to 0-5VDC
      float temperature = 1000.000*sensorValueT;                    //Convert Temperature Sensor 0-5VDC to Kelvin
      Serial.print(temperature);  //Output Tempature to Serial Port
      Serial.println(" K");
    }
  }


  
  
}
