// Derived from Github : https://github.com/kriswiner/IIS3DWB/tree/master/IIS3DWB_Basic_Dragonfly

#include "IIS3DWB.h"
#include "SPI.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_wdt.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Arduino.h>

#define SerialDebug true  // set to true to get Serial output for debugging
#define myLed    2
#define CSPIN    5
///////////////WiFi-Setup//////////////
const char* ssid = "IITB_IOT";
const char* password =  "iitbiot1234";

String control_val = "";

AsyncWebServer server(80);
///////////////IIS3DWB-GET//////////////
TaskHandle_t IIS3DWB_GET_h = NULL;
///////////////IP-ADDRESS SETTINGS//////////////
IPAddress staticIP(192, 168, 0, 188);

IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(0, 0, 0, 0);

//IIS3DWB definitions
// #define IIS3DWB_intPin1 34  // interrupt1 pin definitions, data ready
// #define IIS3DWB_intPin2 35  // interrupt2 pin definitions, activity detection

/* Specify sensor parameters (sample rate is same as the bandwidth 6.3 kHz by default)
 * choices are:  AFS_2G, AFS_4G, AFS_8G, AFS_16G  
*/ 
uint8_t Ascale = AFS_4G;

float aRes;                              // scale resolutions per LSB for the accel 
float accelBias[3] = {0.0f, 0.0f, 0.0f}; // offset biases for the accel 
int16_t IIS3DWBData[3] = {0};                  // Stores the 16-bit signed sensor output
int16_t ax, ay, az;          // variables to hold latest accel data values 
uint8_t IIS3DWBstatus;
const float acc_mult_factor = 1000.0; //convert to mili g

volatile bool IIS3DWB_DataReady = false, IIS3DWB_Wakeup = false;

IIS3DWB IIS3DWB(CSPIN); // instantiate IIS3DWB class

void setup() 
{
  Serial.begin(1500000);
  // Serial.begin(115200);
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

  // c = 0x11;
  // delay(2000);

  if (c != 0x7B) {
    // Serial.end();
    // c = IIS3DWB.getChipID();
  }
  else if(c == 0x7B) // check if all SPI sensors have acknowledged
  {
   //Serial.println("IIS3DWB is online...");  
   //Serial.println(" ");

   // reset IIS3DWB to start fresh
   IIS3DWB.reset();
   delay(1000);
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
  // else 
  // {
  // // if(c != 0x6A) //Serial.println(" IIS3DWB not functioning!"); 
  // while(1){};
  // }

 // attachInterrupt(IIS3DWB_intPin1, myinthandler1, RISING);   // define interrupt for intPin1 output of IIS3DWB
 // attachInterrupt(IIS3DWB_intPin2, myinthandler2, FALLING);  // define interrupt for intPin2 output of IIS3DWB

     xTaskCreatePinnedToCore(
        IIS3DWB_GET,     //TASK
        "IIS3DWB_GET",   //Task id
        8000,               //Stack Size
        NULL,
        1,
        &IIS3DWB_GET_h,   //TASK HANDLE
        1
    );
}
/* End of setup */

void IIS3DWB_GET(void *parameter){

//     if(IIS3DWB_DataReady)  // Handle data ready condition
//     {
//       IIS3DWB_DataReady = false;
    for(;;){  
     IIS3DWBstatus = IIS3DWB.DRstatus(); // read data ready status
     if (IIS3DWBstatus & 0x01) {         // if new accel data is available, read it

        IIS3DWB.readAccelData(IIS3DWBData);  
  
        // Now we'll calculate the accleration value into actual g's
        ax = acc_mult_factor*((float)IIS3DWBData[0]*aRes - accelBias[0]);  // get actual g value in mg, this depends on scale being set
        ay = acc_mult_factor*((float)IIS3DWBData[1]*aRes - accelBias[1]);   
        az = acc_mult_factor*((float)IIS3DWBData[2]*aRes - accelBias[2]);  
        Serial.write(0xAA); // Send the start/sync byte
        Serial.write((uint8_t*)&(ax), sizeof(ax));
        Serial.write((uint8_t*)&(ay), sizeof(ay));
        Serial.write((uint8_t*)&(az), sizeof(az));
      }
    }
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


// void myinthandler1()
// {
//   //IIS3DWB_DataReady = true;
//    IIS3DWB.readAccelData(IIS3DWBData);  
  
//    // Now we'll calculate the accleration value into actual g's
//    ax = 1000.0*((float)IIS3DWBData[0]*aRes - accelBias[0]);  // get actual g value in mg, this depends on scale being set
//    ay = 1000.0*((float)IIS3DWBData[1]*aRes - accelBias[1]);   
//    az = 1000.0*((float)IIS3DWBData[2]*aRes - accelBias[2]);  
//    Serial.write(0xAA); // Send the start/sync byte
//    Serial.write((uint8_t*)&(ax), sizeof(ax));
//    Serial.write((uint8_t*)&(ay), sizeof(ay));
//    Serial.write((uint8_t*)&(az), sizeof(az));
// }

////
//void myinthandler2()
//{
//  IIS3DWB_Wakeup = true;
//}

void loop() {
  // vTaskResume(MPU_GET_h);
 
  init_wifi_server();

  //infite task loop
  for(;;){
    delay(1000);
    yield();
    // //vTaskDelete(NULL)
    connect_to_wifi();
  }

}

void init_wifi_server() {

  if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
    // Serial.println("Configuration failed.");
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); 
  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
  }
   
  server.on(
    "/post",
    HTTP_POST,
    [](AsyncWebServerRequest * request){},
    NULL,
    [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {

    for (size_t i = 0; i < len; i++) {
      control_val += (char)data[i];
    }

    esp_task_wdt_init(10, false);

    // if(control_val == "ESP_RESTART"){
    if(control_val == "ESP_RESTART"){ 
        request->send(200);
        delay(500);
        ESP.restart();
        // vTaskSuspend(MPU_GET_h);
        // delay(500);
        // calibrate_MPU(Acc_bias,Gyro_bias);
        // delay(500);
        // vTaskResume(MPU_GET_h);
      }
      
    control_val = "";
  });

  server.begin();  
}

void connect_to_wifi() {
  unsigned long lastTimeStamp = millis();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
//     Serial.println("Connecting to WiFi..");

    //if WiFI doesn't connect within 30 secs, reconnect WiFi
    if ( (millis() - lastTimeStamp) > 30*1000 ) {  
//       Serial.println("Failed to connect to WiFi. Restarting in 5 seconds");
      delay(5000);
      // ESP.restart();
    WiFi.reconnect();
    }
  }
}
