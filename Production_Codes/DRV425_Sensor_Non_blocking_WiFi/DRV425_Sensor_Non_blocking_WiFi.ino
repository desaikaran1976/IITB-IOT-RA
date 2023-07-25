#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_wdt.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Arduino.h>

#define read_pin 34

const int8_t SYNC_BYTE = 0xAA;
int16_t Bvalue = 0;
const int16_t NUM_OF_CALIBRATION_SAMPLES = 5000;
int32_t B_Value_bias = 0;
int16_t B_bias_int16 = 0;

///////////////WiFi-Setup//////////////
const char* ssid = "IITB_IOT";
const char* password =  "iitbiot1234";

String control_val = "";

AsyncWebServer server(80);
///////////////DRV-GET//////////////
TaskHandle_t DRV_GET_h = NULL;


IPAddress staticIP(192, 168, 0, 187);

IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(0, 0, 0, 0);

void DRV_GET(void *parameter){
  esp_task_wdt_delete(NULL);
  // vTaskSuspend(NULL);
  calibrate_DRV();
  for(;;){
    esp_task_wdt_init(10, false);
    // int16_t s = analogRead(read_pin);
    //Bvalue = ((analogRead(read_pin) - 2048) * 3.3)/4096;
    // B_bias_16 = int16_t(B_Value_bias);
    Bvalue = analogRead(read_pin) - B_bias_int16;


    Serial.write(SYNC_BYTE); // Send the start/sync byte
    // Serial.write((uint8_t*)&(B_bias_16), sizeof(B_bias_16));
    // Serial.write((uint8_t*)&(s), sizeof(s));
    Serial.write((uint8_t*)&(Bvalue), sizeof(Bvalue));
  }
}
///////////////Setup//////////////
void setup() {
  Serial.begin(1500000);
  // Serial.begin(115200);

  
    xTaskCreatePinnedToCore(
        DRV_GET,     //TASK
        "DRV_GET",   //Task id
        8000,               //Stack Size
        NULL,
        1,
        &DRV_GET_h,   //TASK HANDLE
        1
    ); 
}

void calibrate_DRV(){  //pass array by ref
  
  const int MINIMUM_SAMPLING_DELAY_uSEC = 50;  //100 because max sampling rate of accelerometer is 4 khz
  
  B_Value_bias = 0;
  
  for(int i=0; i<NUM_OF_CALIBRATION_SAMPLES; i++){
    
    B_Value_bias = B_Value_bias + (analogRead(read_pin)-2048); //  Avg Bias wrt 2048 which is VCC/2
    delayMicroseconds(MINIMUM_SAMPLING_DELAY_uSEC); //delay because max sampling rate of accelerometer is 4 khz

  }
  
  B_Value_bias = B_Value_bias/NUM_OF_CALIBRATION_SAMPLES; // Int16_t value of B_Value_bias required thus float operation not performed
  B_bias_int16 = int16_t(B_Value_bias);
}

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
