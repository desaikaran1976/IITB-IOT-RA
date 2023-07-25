#include <Arduino.h>

#define read_pin 34
const int8_t SYNC_BYTE = 0xAA;

unsigned long lastMicros = 0;
unsigned long MINIMUM_SAMPLING_DELAY_uSec = 1000;
int32_t Pressure_value = 0;
int32_t Sensor_ID = -100;//Sensor_ID is dummy value to indentify the sensor by python code
float No_of_element = 0.0;
const int prs_window_size = 30; //prs i.e. pressure sensor
int32_t value[prs_window_size];
int32_t i=0;


int32_t Sum = 0;
int32_t prs_running_avg = 0;
int32_t prs_avg_LR_corrected = 0;

float P_min = 0;
float P_FSR = 101.97;
float C_min = 4; //mA
float C_FSR = 20;
float R = 350; //ohms
float LR_Slope = 0.9897;
float LR_offset = 0.39;
float multi_fac = 10.0;

float Slope = (P_FSR-P_min)/((C_FSR-C_min)*R); //Calculates Slope of mapping
float X_Intercept;

void setup() {
  Serial.begin(1500000);
}

void loop() {
      if((micros() - lastMicros) > MINIMUM_SAMPLING_DELAY_uSec){
    lastMicros = micros();
    int32_t adc_average = 0.0;
    int adc_average_iters = 20;
    for (int k=0; k<adc_average_iters; k++){

      adc_average = adc_average + analogRead(read_pin);
      delayMicroseconds(10);
    }
    adc_average = adc_average/adc_average_iters;

  X_Intercept = (3300.0*adc_average/4095.0)-C_min*R;
  Pressure_value = ((X_Intercept*Slope)+ P_min); //Further formula will be modified accordingly
  
  if(No_of_element<prs_window_size){
    Sum+=Pressure_value;
    value[i] = Pressure_value;
    No_of_element = No_of_element+1;
  }
  else{
    Sum+=Pressure_value - value[i]; //circular buffer
    value[i] = Pressure_value;  //circular buffer update
  }
  
    prs_running_avg = Sum/No_of_element;
    prs_avg_LR_corrected = multi_fac*(LR_Slope*float(prs_running_avg) + LR_offset);
    Serial.write(SYNC_BYTE);
    Serial.write((uint8_t*)&prs_avg_LR_corrected, sizeof(prs_avg_LR_corrected));

    Serial.write((uint8_t*)&Sensor_ID, sizeof(Sensor_ID)); 
    i = (i+1)%prs_window_size; //circular buffer position update
  }

}
