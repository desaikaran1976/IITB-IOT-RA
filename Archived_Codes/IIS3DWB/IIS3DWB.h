#ifndef IIS3DWB_h
#define IIS3DWB_h

#include "Arduino.h"
#include <SPI.h>

/* IIS3DWB registers
 https://www.st.com/resource/en/datasheet/iis3dwb.pdf
 */
 
#define IIS3DWB_PIN_CTRL                  0x02
#define IIS3DWB_FIFO_CTRL1                0x07
#define IIS3DWB_FIFO_CTRL2                0x08
#define IIS3DWB_FIFO_CTRL3                0x09
#define IIS3DWB_FIFO_CTRL4                0x0A
#define IIS3DWB_COUNTER_BDR_REG1          0x0B
#define IIS3DWB_COUNTER_BDR_REG2          0x0C
#define IIS3DWB_INT1_CTRL                 0x0D
#define IIS3DWB_INT2_CTRL                 0x0E
#define IIS3DWB_WHO_AM_I                  0x0F  // Different in STM code(0x7B) and datasheet
#define IIS3DWB_CTRL1_XL                  0x10
#define IIS3DWB_CTRL3_C                   0x12
#define IIS3DWB_CTRL4_C                   0x13
#define IIS3DWB_CTRL5_C                   0x14
#define IIS3DWB_CTRL6_C                   0x15
#define IIS3DWB_CTRL7_C                   0x16
#define IIS3DWB_CTRL8_XL                  0x17
#define IIS3DWB_CTRL10_C                  0x19
#define IIS3DWB_ALL_INT_SRC               0x1A
#define IIS3DWB_WAKE_UP_SRC               0x1B
#define IIS3DWB_STATUS_REG                0x1E
#define IIS3DWB_OUT_TEMP_L                0x20
#define IIS3DWB_OUT_TEMP_H                0x21
#define IIS3DWB_OUTX_L_XL                 0x28
#define IIS3DWB_OUTX_H_XL                 0x29
#define IIS3DWB_OUTY_L_XL                 0x2A
#define IIS3DWB_OUTY_H_XL                 0x2B
#define IIS3DWB_OUTZ_L_XL                 0x2C
#define IIS3DWB_OUTZ_H_XL                 0x2D
#define IIS3DWB_FIFO_STATUS1              0x3A
#define IIS3DWB_FIFO_STATUS2              0x3B
#define IIS3DWB_TIMESTAMP0                0x40
#define IIS3DWB_TIMESTAMP1                0x41
#define IIS3DWB_TIMESTAMP2                0x42
#define IIS3DWB_TIMESTAMP3                0x43
#define IIS3DWB_SLOPE_EN                  0x56
#define IIS3DWB_INTERRUPTS_EN             0x58
#define IIS3DWB_WAKE_UP_THS               0x5B
#define IIS3DWB_WAKE_UP_DUR               0x5C
#define IIS3DWB_MD1_CFG                   0x5E
#define IIS3DWB_MD2_CFG                   0x5F
#define IIS3DWB_INTERNAL_FREQ_FINE        0x63
#define IIS3DWB_X_OFS_USR                 0x73
#define IIS3DWB_Y_OFS_USR                 0x74
#define IIS3DWB_Z_OFS_USR                 0x75
#define IIS3DWB_FIFO_DATA_OUT_TAG         0x78
#define IIS3DWB_FIFO_DATA_OUT_X_L         0x79
#define IIS3DWB_FIFO_DATA_OUT_X_H         0x7A
#define IIS3DWB_FIFO_DATA_OUT_Y_L         0x7B
#define IIS3DWB_FIFO_DATA_OUT_Y_H         0x7C
#define IIS3DWB_FIFO_DATA_OUT_Z_L         0x7D
#define IIS3DWB_FIFO_DATA_OUT_Z_H         0x7E


#define AFS_2G  0x00
#define AFS_4G  0x02
#define AFS_8G  0x03
#define AFS_16G 0x01

#define BW_4    0x00   // divide ODR by 4
#define BW_10   0x01
#define BW_20   0x02
#define BW_45   0x03
#define BW_100  0x04
#define BW_200  0x05
#define BW_400  0x06
#define BW_800  0x07


class IIS3DWB
{
  public:
  IIS3DWB(uint8_t cspin);
  float getAres(uint8_t Ascale);
  uint8_t getChipID();
  void sleep();
  void wake();
  void init(uint8_t Ascale);
  void initFIFO(uint16_t fifo_size, uint8_t fifo_mode);
  uint16_t FIFOstatus();
  void offsetBias(float * destination);
  void reset();
  void selfTest();
  void readAccelData(int16_t * destination);
  int16_t readTempData();
  void readFIFOData(uint16_t fifo_count, int16_t * xyzData);
  uint8_t DRstatus();
  uint8_t ACTstatus();

  private:
  uint8_t _cs, _mode;
  void writeByte(uint8_t reg, uint8_t value);
  uint8_t readByte(uint8_t reg);
  void readBytes(uint8_t reg, uint8_t count, uint8_t * dest); 
  float _aRes;
};

#endif
