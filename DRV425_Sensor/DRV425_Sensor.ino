#include <Arduino.h>

#define read_pin 34

const int8_t SYNC_BYTE = 0xAA;

float Bvalue = 0;

void setup() {
  Serial.begin(1000000);
}

void loop() {

  Bvalue = ((analogRead(read_pin) - 2048) * 5)/4096;
  Serial.write(SYNC_BYTE); // Send the start/sync byte
  Serial.write((uint8_t*)&(Bvalue), sizeof(Bvalue));

}
