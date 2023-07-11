#include <Arduino.h>

#define read_pin 34

const int8_t SYNC_BYTE = 0xAA;

int16_t Bvalue = 0;

void setup() {
  Serial.begin(1500000);
}

void loop() {

  //Bvalue = ((analogRead(read_pin) - 2048) * 3.3)/4096;
  Bvalue = analogRead(read_pin);
  Serial.write(SYNC_BYTE); // Send the start/sync byte
  Serial.write((uint8_t*)&(Bvalue), sizeof(Bvalue));

}
