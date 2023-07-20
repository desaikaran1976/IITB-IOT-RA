#include <Arduino.h>

#define read_pin 34
const int8_t SYNC_BYTE = 0xAA;

unsigned long lastMicros = 0;
unsigned long MINIMUM_SAMPLING_DELAY_uSec = 1000;
uint32_t Pressure_value = 0;
uint32_t Sensor_ID = -100;//Sensor_ID is dummy value to indentify the sensor by python code

float P_min = 0;
float P_FSR = 101.97;
float C_min = 4; //mA
float C_FSR = 20;
float R = 350; //ohms

float Slope = (P_FSR-P_min)/((C_FSR-C_min)*R); //Calculates Slope of mapping
float X_Intercept;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(1500000);
}

void loop() {
  // put your main code here, to run repeatedly:
  X_Intercept = (3300.0*analogRead(read_pin)/4095.0)-C_min*R;
  Pressure_value = 1000*((X_Intercept*Slope)+ P_min); //Further formula will be modified accordingly
  
  if((micros() - lastMicros) > MINIMUM_SAMPLING_DELAY_uSec){
    lastMicros = micros();
    Serial.write(SYNC_BYTE);
    Serial.write((uint8_t*)&Pressure_value, sizeof(Pressure_value));

    Serial.write((uint8_t*)&Sensor_ID, sizeof(Sensor_ID)); 
    
  }
}
