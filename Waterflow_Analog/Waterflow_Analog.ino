#include <Arduino.h>

#define read_pin 34
const int8_t SYNC_BYTE = 0xAA;

unsigned long lastMicros = 0;
unsigned long MINIMUM_SAMPLING_DELAY_uSec = 1000;
uint32_t Waterflow_value = 0;
uint32_t Sensor_ID = -1000;//Sensor_ID is dummy value to indentify the sensor by python code

float F_min = 0;
float F_FSR = 500;
float F_FSR1 = 800;
float C_min = 3.375; //mA
float C_FSR = 20;
float R = 240; //ohms

// float Slope = (F_FSR-F_min)/((C_FSR-C_min)*R); //Calculates Slope of mapping for 500l/min range
float Slope1 = (F_FSR1-F_min)/((C_FSR-C_min)*R); //Calculates Slope of mapping for 800l/min range
float X_Intercept;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(1500000);
}

void loop() {
  // put your main code here, to run repeatedly:

  X_Intercept = (3300.0*analogRead(read_pin)/4095.0)-C_min*R;
  // Waterflow_value = 1000*((X_Intercept*Slope)+ F_min); //for 500l/min range Further formula will be modified accordingly
  Waterflow_value = ((X_Intercept*Slope1)+ F_min); // for 800l/min range Further formula will be modified accordingly
  
  if((micros() - lastMicros) > MINIMUM_SAMPLING_DELAY_uSec){
    lastMicros = micros();
    Serial.write(SYNC_BYTE);
    Serial.write((uint8_t*)&Waterflow_value, sizeof(Waterflow_value));

    Serial.write((uint8_t*)&Sensor_ID, sizeof(Sensor_ID)); 

  }
}
