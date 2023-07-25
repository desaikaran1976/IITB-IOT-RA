/*
  example5-BasicReadings-SPI

  This example shows the basic settings and functions for retrieving accelerometer
  data using SPI.
  Please refer to the header file for more possible settings, found here:
  ..\SparkFun_KX13X_Arduino_Library\src\sfe_kx13x_defs.h

  Written by Elias Santistevan @ SparkFun Electronics, October 2022

  Product:

    https://www.sparkfun.com/products/17871

  Repository:

    https://github.com/sparkfun/SparkFun_KX13X_Arduino_Library

  SparkFun code, firmware, and software is released under the MIT
  License  (http://opensource.org/licenses/MIT).
*/

#include <SPI.h>
#include <SparkFun_KX13X.h> // Click here to get the library: http://librarymanager/All#SparkFun_KX13X

SparkFun_KX132_SPI kxAccel;
// SparkFun_KX134_SPI kxAccel; // For the KX134, uncomment this and comment line above

rawOutputData myData;   // Struct for the accelerometer's data
#define chipSelect 5
//byte chipSelect = 5; // Change to fit your project.

void setup()
{

  // Get the chip select pin ready.
  pinMode(chipSelect, OUTPUT);
  digitalWrite(chipSelect, HIGH);
  SPISettings kxSettings;
  SPI.begin();
  kxSettings=SPISettings(10000000,MSBFIRST,SPI_MODE0); //SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
  Serial.begin(1500000);
  //  Serial.println("Welcome.");

  // Wait for the Serial monitor to be opened.
  while (!Serial)
    delay(50);

  if (!kxAccel.begin(SPI,kxSettings,chipSelect))
  {
    //Serial.println("Could not communicate with the the KX13X. Freezing.");
    while (1)
      ;
  }

  //  Serial.println("Ready.");

  // Reset the chip so that old settings don't apply to new setups.
  if (kxAccel.softwareReset())
    //Serial.println("Reset.");

  // Give some time for the accelerometer to reset.
  // It needs two, but give it five for good measure.
  delay(5);

  // Many settings for KX13X can only be
  // applied when the accelerometer is powered down.
  // However there are many that can be changed "on-the-fly"
  // check datasheet for more info, or the comments in the
  // "...regs.h" file which specify which can be changed when.
  kxAccel.enableAccel(false);

  kxAccel.setRange(SFE_KX132_RANGE8G); // 16g Range
  // kxAccel.setRange(SFE_KX134_RANGE16G);         // 16g for the KX134

  kxAccel.enableDataEngine(); // Enables the bit that indicates data is ready.
  kxAccel.setOutputDataRate(15); // Default is 50Hz
  kxAccel.enableAccel();
}

void loop()
{
  // Check if data is ready.
  if (kxAccel.dataReady())
  {
    kxAccel.getRawAccelRegisterData(&myData);
    //    Serial.print("X: ");
//
    myData.xData = (myData.xData-1234)* 0.000244*1000.0;
    myData.yData = (myData.yData-1234)* 0.000244*1000.0;
    myData.zData = (myData.zData-1234)* 0.000244*1000.0;
    
    Serial.write(0xAA);
    Serial.write((uint8_t*)&myData.xData, sizeof(myData.xData));
    //    Serial.print(" Y: ");
    Serial.write((uint8_t*)&myData.yData, sizeof(myData.yData));

    //    Serial.print(" Z: ");
    Serial.write((uint8_t*)&myData.zData, sizeof(myData.zData));
    //    Serial.println();
  }
  //  delay(20); // Delay should be 1/ODR (Output Data Rate), default is 1/50ODR
}
