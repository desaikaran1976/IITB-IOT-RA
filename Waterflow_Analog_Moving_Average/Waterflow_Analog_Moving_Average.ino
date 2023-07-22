#include <Arduino.h>
// #include <cppQueue.h>
// #include <queue>

#define read_pin 34
const int8_t SYNC_BYTE = 0xAA;

unsigned long lastMicros = 0;
unsigned long MINIMUM_SAMPLING_DELAY_uSec = 1000;
int32_t Waterflow_value = 0;
int32_t Sensor_ID = -1000;//Sensor_ID is dummy value to indentify the sensor by python code
float No_of_element = 0.0;
const int wtf_window_size = 30; //wtf i.e. waterflow
int32_t value[wtf_window_size];
int32_t i=0;


int32_t Sum = 0;
int32_t wtf_running_avg = 0;


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
    if((micros() - lastMicros) > MINIMUM_SAMPLING_DELAY_uSec){
    lastMicros = micros();
    int32_t adc_average = 0.0;
    int adc_average_iters = 20;
    for (int k=0; k<adc_average_iters; k++){

      adc_average = adc_average + analogRead(read_pin);
      delayMicroseconds(10);
    }
    adc_average = adc_average/adc_average_iters;
    // int32_t a = int32_t(adc_average);

  // put your main code here, to run repeatedly:
  X_Intercept = (3300.0*adc_average/4095.0)-C_min*R;
  // Waterflow_value = 1000*((X_Intercept*Slope)+ F_min); //for 500l/min range Further formula will be modified accordingly
  Waterflow_value = ((X_Intercept*Slope1)+ F_min); // for 800l/min range Further formula will be modified accordingly

  // value[i] = Waterflow_value;
  // head = (i)%10;
  // tail = (9-i)%10;
  if(No_of_element<wtf_window_size){
    Sum+=Waterflow_value;
    value[i] = Waterflow_value;
    No_of_element = No_of_element+1;
  }
  else{
    Sum+=Waterflow_value - value[i]; //circular buffer
    value[i] = Waterflow_value;  //circular buffer update
  }
  
    wtf_running_avg = Sum/No_of_element;
    Serial.write(SYNC_BYTE);
    Serial.write((uint8_t*)&wtf_running_avg, sizeof(wtf_running_avg));

    Serial.write((uint8_t*)&Sensor_ID, sizeof(Sensor_ID)); 
    i = (i+1)%wtf_window_size; //circular buffer position update
  }

}
