#include <Arduino.h>

#define read_pin 34
const int8_t SYNC_BYTE = 0xAA

unsigned long lastMicros = 0
unsigned long MINIMUM_SAMPLING_DELAY_uSec = 1000;
int16_t Pressure_value = 0;
int16_t Sensor_ID = -100;//Sensor_ID is dummy value to indentify the sensor by python code

void setup() {
  // put your setup code here, to run once:
  Serial.begin(1500000);
}

void loop() {
  // put your main code here, to run repeatedly:
  Pressure_Value = analogRead(read_pin); //Further formula will be modified accordingly
  
  if((micros() - lastMicros) > MINIMUM_SAMPLING_DELAY_uSec){
    Serial.write(SYNC_BYTE);
    Serial.write((uint8_t*)&Pressure_value, sizeof(Pressure_value));

    Serial.write(SYNC_BYTE);
    Serial.write((uint8_t*)&Sensor_ID, sizeof(Sensor_ID)); 
    lastMicros = micros();
  }
}
