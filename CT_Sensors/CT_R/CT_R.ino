#include <Arduino.h>

#define read_pin 34

const int8_t SYNC_BYTE = 0xAA;

int16_t CT_value = 0;

void setup() {
  Serial.begin(1500000);
}

void loop() {

  CT_value = analogRead(read_pin)+5000;
  Serial.write(SYNC_BYTE); // Send the start/sync byte
  Serial.write((uint8_t*)&(CT_value), sizeof(CT_value));

}
