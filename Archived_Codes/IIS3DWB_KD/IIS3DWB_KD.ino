#include "IIS3DWB_KD.h"





/************  Constructors ************/

IIS3DWB_KD::IIS3DWB_KD(SPIClass *s, int cs, bool spi)
    : _spi(s)
    , csPin(cs)
    , useSPI(spi)
{
    // intentionally empty
}

/************ Basic Settings ************/

bool IIS3DWB_KD::init(uint8_t const expectedValue){
  if(useSPI){
    pinMode(csPin,OUTPUT);
    digitalWrite(csPin,HIGH);
    _spi->begin();
    mySPISettings = SPISettings(10000000,MSBFIRST, SPI_MODE0); //10Mhz SPI max speed
  }
  reset();
  delay(10);
  mySensorSettings(); 


}

bool IIS3DWB_KD::init(){
  return init(REGISTER_WHO_AM_I);
}

void IIS3DWB_KD::reset(){
  writeByte(REGISTER_CTRL1_XL, 0x00); // set accel to power down mode
  uint8_t temp = readByte(REGISTER_CTRL3_C); 
  writeByte(REGISTER_CTRL3_C, temp | 0x01); // Set bit 0 to 1 to reset IIS3DWB
  delay(1); // Wait for all registers to reset
}

void IIS3DWB_KD::mySensorSettings(){
  writeByte(REGISTER_INT1_CTRL, 0x01);          // enable data ready interrupt on INT1
  writeByte(REGISTER_COUNTER_BDR_REG1, 0x80);   // enable pulsed (not latched) data ready interrupt  

  // enable block update (bit 6 = 1), auto-increment registers (bit 2 = 1)
  writeByte(REGISTER_CTRL3_C, 0x40 | 0x04);
  // by default, interrupts active HIGH, push pull 
  // (can be changed by writing to bits 5 and 4, resp to above register)

  //  mask data ready until filter settle complete (bit 3 == 1), disable I2C (bit 2 == 1)
  writeByte(REGISTER_CTRL4_C, 0x08 | 0x04);

  writeByte(REGISTER_CTRL1_XL, 0xA0 | Ascale << 2); // set accel full scale and enable accel

  // High pass filter selection, comment out for full 6.3 kHz bandwidth
  // Set HPF to ODR/800 (bits 5 - 7 == 1), set HP Ref Mode (bit 4 == 1)
  // Set HP fast settle mode (bit 3 == 1), set filter select to 1 (bit 2 == 1)
  //  writeByte(REGISTER_CTRL8_XL, 0xFC);  
  writeByte(REGISTER_CTRL8_XL, 0xEC);  // Don't set HP reference mode

  // activity interrupt handling
  writeByte(REGISTER_WAKE_UP_DUR, 0x08);        // set inactivity duration at 1 LSB = 512/26.667 kHz ODR (so about 0.15 seconds)
  writeByte(REGISTER_WAKE_UP_THS, 0x02);        // set wake threshold to 62.5 mg at 4 G FS, 4G/2^6 = 0.0625G
  // (change SLOPE_EN to 0x00 to drive activity change to INT)
  writeByte(REGISTER_SLOPE_EN, 0x20);           // drive activity status to interrupt      
  writeByte(REGISTER_INTERRUPTS_EN, 0x80);      // enable wakeup and activity/inactivity logic
  writeByte(REGISTER_MD2_CFG, 0x80);            // route activity change event to INT2
}

/************************************************
     Private Functions
*************************************************/

void IIS3DWB_KD::writeByte(uint8_t reg, uint8_t val){
  if(useSPI){
    _spi->beginTransaction(mySPISettings);
    digitalWrite(csPin, LOW);
    _spi->transfer(reg & 0x7F); //as MSB for RW mode is 0 (check basis) 
    _spi->transfer(val);
    digitalWrite(csPin, HIGH);
    _spi->endTransaction();
  }
  //return;
}