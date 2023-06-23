// Derived from Github : https://github.com/kriswiner/IIS3DWB/tree/master/IIS3DWB_Basic_Dragonfly

#include "IIS3DWB.h"
#include "SPI.h"

#define SerialDebug true  // set to true to get Serial output for debugging
#define myLed    2
#define CSPIN    5


//IIS3DWB definitions
#define IIS3DWB_intPin1 34  // interrupt1 pin definitions, data ready
#define IIS3DWB_intPin2 35  // interrupt2 pin definitions, activity detection

/* Specify sensor parameters (sample rate is same as the bandwidth 6.3 kHz by default)
 * choices are:  AFS_2G, AFS_4G, AFS_8G, AFS_16G  
*/ 
uint8_t Ascale = AFS_4G;

float aRes;                              // scale resolutions per LSB for the accel 
float accelBias[3] = {0.0f, 0.0f, 0.0f}; // offset biases for the accel 
int16_t IIS3DWBData[3] = {0};                  // Stores the 16-bit signed sensor output
int16_t ax, ay, az;          // variables to hold latest accel data values 
uint8_t IIS3DWBstatus;

volatile bool IIS3DWB_DataReady = false, IIS3DWB_Wakeup = false;

IIS3DWB IIS3DWB(CSPIN); // instantiate IIS3DWB class

void setup() 
{
  Serial.begin(1500000);
//  delay(4000);

  SPI.begin(18,19,23,5); // Start SPI serial peripheral(CLK,MISO,MOSI,CS)

  // Configure led
  pinMode(myLed, OUTPUT);
  digitalWrite(myLed, HIGH); // start with led off
  
//  delay(20); // wait at least 10 ms for IIS3DWB boot procedure to complete

  // Configure SPI ship select for sensor breakout
  pinMode(CSPIN, OUTPUT);
  digitalWrite(CSPIN, HIGH); // disable SPI at start
 
  // Configure interrupt pins
  //pinMode(IIS3DWB_intPin1, INPUT); // enable IIS3DWB interrupt1
 // pinMode(IIS3DWB_intPin2, INPUT); // enable IIS3DWB interrupt2

  // Read the IIS3DWB Chip ID register, this is a good test of communication
  //Serial.println("IIS3DWB accel...");
  uint8_t c = IIS3DWB.getChipID();  // Read CHIP_ID register for IIS3DWB
//  Serial.print("IIS3DWB "); Serial.print("I AM "); Serial.print(c, HEX); Serial.print(" I should be "); Serial.println(0x7B, HEX);
  //Serial.println(" ");

  if(c == 0x7B) // check if all SPI sensors have acknowledged
  {
   //Serial.println("IIS3DWB is online...");  
   //Serial.println(" ");

   // reset IIS3DWB to start fresh
   IIS3DWB.reset();
   delay(3000); 
   digitalWrite(myLed, LOW); // indicate passed the ID check

   // get accel sensor resolution, only need to do this once
   aRes = IIS3DWB.getAres(Ascale);

//   IIS3DWB.selfTest();
    
   IIS3DWB.init(Ascale); // configure IIS3DWB  

   IIS3DWB.offsetBias(accelBias);
   //Serial.println("accel biases (mg)"); Serial.println(1000.0f * accelBias[0]); Serial.println(1000.0f * accelBias[1]); Serial.println(1000.0f * accelBias[2]);
   //Serial.println(" ");
   delay(10); 
   
   //digitalWrite(myLed, HIGH); // turn off led when sensor configuration is finished
  }
  else 
  {
  if(c != 0x6A) //Serial.println(" IIS3DWB not functioning!"); 
  while(1){};
  }

  attachInterrupt(IIS3DWB_intPin1, myinthandler1, RISING);   // define interrupt for intPin1 output of IIS3DWB
 // attachInterrupt(IIS3DWB_intPin2, myinthandler2, FALLING);  // define interrupt for intPin2 output of IIS3DWB
}
/* End of setup */

void loop() {

//     if(IIS3DWB_DataReady)  // Handle data ready condition
//     {
//       IIS3DWB_DataReady = false;
      
//      IIS3DWBstatus = IIS3DWB.DRstatus(); // read data ready status
//      if (IIS3DWBstatus & 0x01) {         // if new accel data is available, read it

//         IIS3DWB.readAccelData(IIS3DWBData);  
//   
//         // Now we'll calculate the accleration value into actual g's
//         ax = 1000.0*((float)IIS3DWBData[0]*aRes - accelBias[0]);  // get actual g value in mg, this depends on scale being set
//         ay = 1000.0*((float)IIS3DWBData[1]*aRes - accelBias[1]);   
//         az = 1000.0*((float)IIS3DWBData[2]*aRes - accelBias[2]);  
//         Serial.write(0xAA); // Send the start/sync byte
//         Serial.write((uint8_t*)&(ax), sizeof(ax));
//         Serial.write((uint8_t*)&(ay), sizeof(ay));
//         Serial.write((uint8_t*)&(az), sizeof(az));
//       }
   //  }  // end of data ready interrupt handling
     
//
//      if (IIS3DWB_Wakeup) { // if activity change event FALLING detected
//          IIS3DWB_Wakeup = false;
//        
//          //Serial.println("IIS3DWB is awake");
//     } // end activity change interrupt handling 
     
   // end sensor interrupt handling
    
//    if(SerialDebug) { // report latest accel data
//    Serial.print("ax = "); Serial.print((int)ax);  
//    Serial.print(" ay = "); Serial.print((int)ay); 
//    Serial.print(" az = "); Serial.print((int)az); Serial.println(" mg");
//    }

        

//    IIS3DWBData[3] = IIS3DWB.readTempData(); // get IIS3DWB chip temperature
//    accelTemp = ((float) IIS3DWBData[3]) / 256.0f + 25.0f; // Accel chip temperature in degrees Centigrade
    // Print temperature in degrees Centigrade      
//    if(SerialDebug) {
//      Serial.print("IIS3DWB temperature is ");  Serial.print(accelTemp, 1);  Serial.println(" degrees C"); // Print T values to tenths of a degree C
//    }

//    digitalWrite(myLed, LOW); delay(10); digitalWrite(myLed, HIGH);   // toggle the led
}
/*  End of main loop */


void myinthandler1()
{
  //IIS3DWB_DataReady = true;
   IIS3DWB.readAccelData(IIS3DWBData);  
  
   // Now we'll calculate the accleration value into actual g's
   ax = 1000.0*((float)IIS3DWBData[0]*aRes - accelBias[0]);  // get actual g value in mg, this depends on scale being set
   ay = 1000.0*((float)IIS3DWBData[1]*aRes - accelBias[1]);   
   az = 1000.0*((float)IIS3DWBData[2]*aRes - accelBias[2]);  
   Serial.write(0xAA); // Send the start/sync byte
   Serial.write((uint8_t*)&(ax), sizeof(ax));
   Serial.write((uint8_t*)&(ay), sizeof(ay));
   Serial.write((uint8_t*)&(az), sizeof(az));
}

////
//void myinthandler2()
//{
//  IIS3DWB_Wakeup = true;
//}
