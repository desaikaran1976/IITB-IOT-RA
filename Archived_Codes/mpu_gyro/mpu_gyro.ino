/*

CORE0: MPU9250 data acquisition and serial write done sequentially.
CORE1: ESP Restart over HTTP.

Python File for sending requests: relay_post.py
Python Command: python relay_post.py

Device IP: 192.168.0.110 (Static IP)
Backup IP: 192.168.0.120 (Static IP)

HTTP Link: http://192.168.x.110:80/post

////////HTTP COMMANDS////////
RELAY_OPEN - Switch Open
RELAY_CLOSE - Switch Close
ESP_RESTART - Restart ESP

*/
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_wdt.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#include <MPU9250_WE.h>
#include <SPI.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"

/////////////MPU Setup/////////////
//VSPI
#define MPU_CS_PIN        5

int16_t Gx {0};
int16_t Gy {0};
int16_t Gz {0};

float Acc_bias[3];
float Gyro_bias[3];

const int8_t SYNC_BYTE = 0xAA;
bool useSPI = true;    // SPI use flag
unsigned long lastMicros = 0, lastMicros2 = 0;
const int16_t MAX_SAMPLING_FREQ = 4000;
const int16_t NUM_OF_CALIBRATION_SAMPLES = 5000;
unsigned long MINIMUM_SAMPLING_DELAY_uSec = (unsigned long)(1 * 1000000 / MAX_SAMPLING_FREQ);
const int16_t gyr_factor = 10;
const int16_t acc_factor = 10000;
const int16_t gyro_offset = 10000;

///////////////WiFi-Setup//////////////
const char* ssid = "IITB_IOT";
const char* password =  "iitbiot1234";

String control_val = "";

AsyncWebServer server(80);
IPAddress staticIP(192, 168, 0, 185);

IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(0, 0, 0, 0);

///////////////CONSTRUCTORS//////////////
MPU9250_WE myMPU9250 = MPU9250_WE(&SPI, MPU_CS_PIN, useSPI);    //VSPI

///////////////FUNCTION-PROTO////////////
void calibrate_MPU(float acc_bias[], float gyro_bias[]);
void connect_to_wifi();

/////////////FREE-RTOS Setup/////////////
TaskHandle_t MPU_GET_h = NULL;

void MPU_GET( void * parameter ) {
  esp_task_wdt_delete(NULL);

  // vTaskSuspend(NULL);

  myMPU9250.init();
  delay(1000);
  myMPU9250.autoOffsets();
  myMPU9250.disableGyrDLPF(MPU9250_BW_WO_DLPF_8800); // bandwdith without DLPF
  myMPU9250.setGyrRange(MPU9250_GYRO_RANGE_500);
  myMPU9250.setAccRange(MPU9250_ACC_RANGE_4G);
  myMPU9250.enableAccDLPF(false);
  myMPU9250.setAccDLPF(MPU9250_DLPF_6);
  myMPU9250.setMagOpMode(AK8963_CONT_MODE_100HZ);
  delay(1000);
  calibrate_MPU(Acc_bias,Gyro_bias);

  for (;;) {
    esp_task_wdt_init(10, false);

   xyzFloat gyr = myMPU9250.getGyrValues();

        Gx = (gyr.x - Gyro_bias[0])*gyr_factor + gyro_offset;
        Gy = (gyr.y - Gyro_bias[1])*gyr_factor + gyro_offset;
        Gz = (gyr.z - Gyro_bias[2])*gyr_factor + gyro_offset;

        Serial.write(SYNC_BYTE); // Send the start/sync byte

        Serial.write((uint8_t*)&(Gx), sizeof(Gx));
        Serial.write((uint8_t*)&(Gy), sizeof(Gy));
        Serial.write((uint8_t*)&(Gz), sizeof(Gz));
        
        
        // Serial.printf("%f %d \n",Acc_bias[2], Az);

//    }
  }
}

void setup() {
     Serial.begin(1500000);
  //  Serial.begin(115200);

    xTaskCreatePinnedToCore(
        MPU_GET,     //TASK
        "MPU_GET",   //Task id
        8000,               //Stack Size
        NULL,
        1,
        &MPU_GET_h,   //TASK HANDLE
        1
    );

    // vTaskResume(MPU_GET_h);
    if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
      // Serial.println("Configuration failed.");
    }    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password); 
  // attempt to connect to Wifi network:
    if (WiFi.status() != WL_CONNECTED) {
        // delay(1000);
    }
  // Serial.println(WiFi.localIP());

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
        }
        
        control_val = "";
    });
  
    server.begin();
}

void loop() {
  delay(1000);
  // vTaskDelete(NULL);
  connect_to_wifi();
}

void connect_to_wifi() {
  unsigned long lastTimeStamp = millis();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    // Serial.println("Connecting to WiFi..");

    //if WiFI doesn't connect within 30 secs, reconnect WiFi
    if ( (millis() - lastTimeStamp) > 30*1000 ) {  
      // Serial.println("Failed to connect to WiFi. Restarting in 5 seconds");
      delay(5000);
      // ESP.restart();
    WiFi.reconnect();
    }
  }
}

void calibrate_MPU(float acc_bias[], float gyro_bias[]) {  //pass array by ref
  
  const int MPU9250_MINIMUM_SAMPLING_DELAY_uSEC = 50;  //250 because max sampling rate of accelerometer is 4 khz
  
  for(int i=0; i<3; i++) {
    gyro_bias[i] = 0.0;
 
  }
  
  for(int i=0; i<NUM_OF_CALIBRATION_SAMPLES; i++){
    xyzFloat gValue = myMPU9250.getGValues();
    xyzFloat gyr = myMPU9250.getGyrValues();
    gyro_bias[0] = gyro_bias[0] + (gyr.x);
    gyro_bias[1] = gyro_bias[1] + (gyr.y);
    gyro_bias[2] = gyro_bias[2] + (gyr.z);
    delayMicroseconds(MPU9250_MINIMUM_SAMPLING_DELAY_uSEC); //delay because max sampling rate of accelerometer is 4 khz

  }
  
  for(int i=0; i<3; i++){
    gyro_bias[i] = gyro_bias[i]/NUM_OF_CALIBRATION_SAMPLES;
  }
}
