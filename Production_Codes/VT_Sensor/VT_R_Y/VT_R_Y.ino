#include <Arduino.h>

#define read_pin_VT_R 34
#define read_pin_VT_Y 32

const int8_t SYNC_BYTE = 0xAA;

int16_t VT_R_value = 0;
int16_t VT_Y_value = 0;

void setup() {
  Serial.begin(1500000);
}

void loop() {

  VT_R_value = analogRead(read_pin_VT_R)+5000;
  VT_Y_value = analogRead(read_pin_VT_Y);
  Serial.write(SYNC_BYTE); // Send the start/sync byte
  Serial.write((uint8_t*)&(VT_R_value), sizeof(VT_R_value));
  Serial.write((uint8_t*)&(VT_Y_value), sizeof(VT_Y_value));
}
