#include <Arduino.h>

#define read_pin_CT 34
#define read_pin_VT 32

const int8_t SYNC_BYTE = 0xAA;

int16_t CT_value = 0;
int16_t VT_value = 0;

void setup() {
  Serial.begin(1500000);
}

void loop() {

  CT_value = analogRead(read_pin_CT);
  VT_value = analogRead(read_pin_VT);
  Serial.write(SYNC_BYTE); // Send the start/sync byte
  Serial.write((uint8_t*)&(CT_value), sizeof(CT_value));
  Serial.write((uint8_t*)&(VT_value), sizeof(VT_value));
}
