#ifndef IIS3DWB_KD_h
#define IIS3DWB_KD_h

#include "Arduino.h"
#include <SPI.h>

class IIS3DWB_KD
{
  public:

    static uint8_t constexpr REGISTER_PIN_CTRL                 = 0x02;

    struct register_pin_ctrl{
      uint8_t SDO_PU_EN = 0x7F; //default is disabled 0x00
    };

    static uint8_t constexpr REGISTER_FIFO_CTRL1               = 0x07;

    struct register_fifo_ctrl1{
      uint8_t  
    };
    static uint8_t constexpr REGISTER_FIFO_CTRL2               = 0x08;
    
    struct register_fifo_ctrl2{
      uint8_t STOP_ON_WTM = 0x80; //default will be FIFO depth is not limited
    };

    static uint8_t constexpr REGISTER_FIFO_CTRL3               = 0x09;
    
    struct register_fifo_ctrl3{
      uint8_t BDR_XL_0 = 0x00; //default acc not batched in FIFO
      uint8_t BDR_XL_EN = 0x0A;
    };

    static uint8_t constexpr REGISTER_FIFO_CTRL4               = 0x0A;
    
    struct register_fifo_ctrl4{
      uint8_t DEC_TS_BATCH_0 = 0x00; //Timestamp not batched in FIFO(default)
      uint8_t DEC_TS_BATCH_1 = 0x40; //decimation 1 
      uint8_t DEC_TS_BATCH_8 = 0x80; //decimation 8
      uint8_t DEC_TS_BATCH_32 = 0xC0; //decimation 32

      uint8_t ODR_T_BATCH_EN = 0x30; //Temperature batched at 104Hz (default not batched)

      uint8_t FIFO_MODE_B = 0x00; // Bypass mode FIFO disabled(default)
      uint8_t FIFO_MODE_FF = 0x01; // FIFO mode
      uint8_t FIFO_MODE_CFF = 0x03; // Continous to FIFO
      uint8_t FIFO_MODE_BC = 0x04; // Bypass to Continous mode
      uint8_t FIFO_MODE_C = 0x06; // Continous mode
      uint8_t FIFO_MODE_BFF = 0x07; // Bypass to Continous mode   
    };

    static uint8_t constexpr REGISTER_COUNTER_BDR_REG1         = 0x0B;

    struct register_counter_bdr_reg1{
      uint8_t dataready_pulsed = 0x80; // dataready pulse mode, default is latched mode
      uint8_t RST_COUNTER_BDR = 0x40; //reset internal counter. Automatically reset 1 -> 0
      uint8_t  
    };

    static uint8_t constexpr REGISTER_COUNTER_BDR_REG2         = 0x0C;

    struct register_counter_bdr_reg2{
      uint8_t 
    };

    static uint8_t constexpr REGISTER_INT1_CTRL                = 0x0D;

    struct register_int1_ctrl{
      uint8_t INT1_CNT_BDR = 0x40; // enable counter_bdr_ia interrupt on INT1 
      uint8_t INT1_FIFO_FULL = 0x20; // enable FIFO full flag interrupt on INT1 pin
      uint8_t INT1_FIFO_OVR = 0x10; // enable FIFO overrun interrupt pn INT1 pin
      uint8_t INT1_FIFO_TH = 0x08; // enable FIFO threshold interrupt on INT1 pin
      uint8_t INT1_BOOT = 0x04; // enable boot status on INT1 pin
      uint8_t INT1_DRDY_XL = 0x01; // enable accelerometer data-ready interrupt on INT1 pin
    };

    static uint8_t constexpr REGISTER_INT2_CTRL                = 0x0E;
    
    struct register_int2_ctrl{
      uint8_t INT2_CNT_BDR = 0x40; // enable counter_bdr_ia interrupt on INT2 
      uint8_t INT2_FIFO_FULL = 0x20; // enable FIFO full flag interrupt on INT2 pin
      uint8_t INT2_FIFO_OVR = 0x10; // enable FIFO overrun interrupt pn INT2 pin
      uint8_t INT2_FIFO_TH = 0x08; // enable FIFO threshold interrupt on INT2 pin
      uint8_t INT2_BOOT = 0x04; // enable boot status on INT2 pin
      uint8_t INT2_DRDY_XL = 0x01; // enable accelerometer data-ready interrupt on INT2 pin
    };
    
    static uint8_t constexpr REGISTER_WHO_AM_I                 = 0x0F;  // Different in STM code(0x7B) and datasheet
    
    static uint8_t constexpr REGISTER_CTRL1_XL                 = 0x10;
    
    struct register_ctrl1_xl{
      uint8_t XL_EN = 0xA0; //Enable Accelerometer
      
      uint8_t FS_XL_2G = 0x00; // 2g range(default)
      uint8_t FS_XL_16G = 0x04; // 16g range 
      uint8_t FS_XL_4G = 0x08; // 4g range
      uint8_t FS_XL_8G = 0x0C; // 8g range

      uint8_t LPF2_XL_EN = 0x02; // output from LPF2 second filtering stage, default is from digital filtering stage 0x00
    };

    static uint8_t constexpr REGISTER_CTRL3_C                  = 0x12;

    struct register_ctrl3_c{
      uint8_t BOOT = 0x80; // reboot memory content, default is normal mode 0x00
      uint8_t BDU = 0x40; // output register not updated until MSB and LSB are read , default is continous update 0x00
      uint8_t H_LACTIVE = 0x20; // Interrupt output pins active low, default is interrupt output pins active high 0x00
      uint8_t PP_OD = 0x10; // open-drain mode, default is push-pull mode 0x00
      uint8_t SIM = 0x08; // 3-wire interface, default is 4-wire inteface 0x00
      uint8_t IF_INC = 0x04; // Enable , default disabled 0x00 ??write better description
      uint8_t SW_RESET = 0x01; // reset device, automatically cleared
    };

    static uint8_t constexpr REGISTER_CTRL4_C                  = 0x13;
    
    struct register_ctrl4_c{
      uint8_t INT2_on_INT1 = 0x20; //all interrupt logic in INT1, default is divided between INT1 and 2 0x00
      uint8_t DRDY_MASK = 0x08; // mask DRDY on pin until filter settling ends, default disabled 0x00
      uint8_t I2C_disable = 0x40; // disable I2C interface, default is SPI and I2C enabled 0x00
      uint8_t _1AX_TO_3REGOUT = 0x01; //single-axis mode, uses the output of the XYZ registers to give 3 consecutive samples of the selected single axis
    };

    static uint8_t constexpr REGISTER_CTRL5_C                  = 0x14;

    struct register_ctrl5_c{
      uint8_t ROUNDING = 0x20; // wraparound enabled, default disabled 0x00

      uint8_t ST_XL_N = 0x00; // Self-test Normal mode
      uint8_t ST_XL_PS = 0x01; // Positive sign self-test
      uint8_t ST_XL_NS = 0x02; // Negative sign self-test
      uint8_t ST_XL_NA = 0x03; // Not Allowed
    };

    static uint8_t constexpr REGISTER_CTRL6_C                  = 0x15;

    struct register_ctrl6_c{
      uint8_t USR_OFF_W_10 = 0x00; // 2^-10 g/LSB weight of offset bit(default)
      uint8_t USR_OFF_W_6 = 0x08; // 2^-6 g/LSB weight of offset bit 

      uint8_t XL_AXIS_SEL_3 = 0x00; // 3 axes(XYZ)(default)
      uint8_t XL_AXIS_SEL_X = 0x01; // X-axis
      uint8_t XL_AXIS_SEL_Y = 0x02; // Y-axis
      uint8_t XL_AXIS_SEL_Z = 0x03; // Z-axis 
    };

    static uint8_t constexpr REGISTER_CTRL7_C                  = 0x16;

    struct register_ctrl7_c{
      uint8_t USR_OFF_ON_OUT = 0x02; // enable accelerometer user offset correction block, default is disabled 0x00
    };

    static uint8_t constexpr REGISTER_CTRL8_XL                 = 0x17;

    struct register_ctrl8_c{
      uint8_t HPCF_XL_ODR4 = 0x00; // Bandwidth ODR/4 ODR = 6.3Hz, for High Pass also called SLOPE
      uint8_t HPCF_XL_ODR10 = 0x20; // Bandwidth ODR/10
      uint8_t HPCF_XL_ODR20 = 0x40; // Bandwidth ODR/20
      uint8_t HPCF_XL_ODR45 = 0x60; // Bandwidth ODR/45
      uint8_t HPCF_XL_ODR100 = 0x80; // Bandwidth ODR/100
      uint8_t HPCF_XL_ODR200 = 0xA0; // Bandwidth ODR/200
      uint8_t HPCF_XL_ODR400 = 0xC0; // Bandwidth ODR/400
      uint8_t HPCF_XL_ODR800 = 0xE0; // Bandwidth ODR/800

      uint8_t HP_REF_MODE_XL = 0x10; // Enables accelerometer high-pass filter reference mode FDS must be 1 HPCF_XL must be ODR/800
      uint8_t FASTSETTL_MODE_XL = 0x08; //Enables accelerometer LPF2 and HPF fast-settling mode, default is 0 0x00
      uint8_t FDS = 0x04; // Accelerometer low-pass / high-pass filter selection, 1 is high pass, default is 0 low pass
    };

    static uint8_t constexpr REGISTER_CTRL10_C                 = 0x19;
    
    struct register_ctrl8_c{
      uint8_t TIMESTAMP_EN = 0x20; // enables timestamp counter, default is disabled 0x00
    };

    static uint8_t constexpr REGISTER_ALL_INT_SRC              = 0x1A;

    struct register_all_int_src{
      uint8_t TIMESTAMP_ENDCOUNT = 0x80; // Alerts timestamp overflow within 6.4ms
      uint8_t SLEEP_CHANGE_IA = 0x20; // Detects change event in activity/inactivity status, 1 change staus detected
      uint8_t WU_IA = 0x02; // Wake_up event status, 1 event detected
    };

    static uint8_t constexpr REGISTER_WAKE_UP_SRC              = 0x1B;
    
    struct register_wake_up_src{
      uint8_t SLEEP_CHANGE_IA = 0x40; // Detects change event in activity/inactivity status
      uint8_t SLEEP_STATE_IA = 0x10; // Sleep event status, 1 sleep event status detected
      uint8_t WU_IA = 0x08; // Wake_up event detection status
      uint8_t X_WU = 0x04; // Wake_up event detection on X-axis 
      uint8_t Y_WU = 0x02; // Wake_up event detection on Y-axis
      uint8_t Z_WU = 0x01; // Wake_up event detection on Z-axis
    };

    static uint8_t constexpr REGISTER_STATUS_REG               = 0x1E;
    
    struct register_status_reg{
      uint8_t TDA = 0x04; // Temperature new data available
      uint8_t XLDA = 0x01; // Accelerometer new data available
    };

    static uint8_t constexpr REGISTER_OUT_TEMP_L               = 0x20;

    struct register_out_temp_l{
      uint8_t 
    };

    static uint8_t constexpr REGISTER_OUT_TEMP_H               = 0x21;

    struct register_out_temp_h{
      uint8_t 
    };

    static uint8_t constexpr REGISTER_OUTX_L_XL                = 0x28;

    struct register_outx_l_xl{
      uint8_t 
    };

    static uint8_t constexpr REGISTER_OUTX_H_XL                = 0x29;
    
    struct register_outx_h_xl{
      uint8_t 
    };    

    static uint8_t constexpr REGISTER_OUTY_L_XL                = 0x2A;
    
    struct register_outy_l_xl{
      uint8_t 
    };        

    static uint8_t constexpr REGISTER_OUTY_H_XL                = 0x2B;
    
    struct register_outy_h_xl{
      uint8_t 
    };

    static uint8_t constexpr REGISTER_OUTZ_L_XL                = 0x2C;
    
    struct register_outz_l_xl{
      uint8_t 
    };

    static uint8_t constexpr REGISTER_OUTZ_H_XL                = 0x2D;
    
    struct register_outz_h_xl{
      uint8_t 
    };

    static uint8_t constexpr REGISTER_FIFO_STATUS1             = 0x3A;

    struct register_fifo_status1{
      uint8_t 
    };

    static uint8_t constexpr REGISTER_FIFO_STATUS2             = 0x3B;
    
    struct register_fifo_status2{
      uint8_t FIFO_WTM_IA = 0x80; // FIFO watermark status, 0 FIFO filling < WTM , 1 FIFO filling >= WTM
      uint8_t FIFO_OVR_IA = 0x40; // FIFO Overrun status
      uint8_t FIFO_FULL_IA = 0x20; // 1 : FIFO is full at the next ODR, 0: FIFO is not full
      uint8_t COUNTER_BDR_IA = 0x10; // Counter BDR reaches the CNT_BDR_TH_[10:0] threshold set, bit is reset when these registers are read
      uint8_t FIFO_OVR_LATCHED = 0x08; // Latched FIFO overrun status, bit is reset when this register is read
      uint8_t 
    };

    static uint8_t constexpr REGISTER_TIMESTAMP0               = 0x40;

    struct register_timestamp0{
      uint8_t
    };

    static uint8_t constexpr REGISTER_TIMESTAMP1               = 0x41;
    
    struct register_timestamp1{
      uint8_t
    };

    static uint8_t constexpr REGISTER_TIMESTAMP2               = 0x42;
    
    struct register_timestamp2{
      uint8_t
    };

    static uint8_t constexpr REGISTER_TIMESTAMP3               = 0x43;
    
    struct register_timestamp3{
      uint8_t
    };

    static uint8_t constexpr REGISTER_SLOPE_EN                 = 0x56;
    
    struct register_slope_en{
      uint8_t SLEEP_STATUS_ON_INT = 0x20; // Activity/inactivity interrupt mode configuration?? better description
      uint8_t SLOPE_FDS = 0x10; // HPF or slope filter selection on wake-up and activity/inactivity functions, 1 HPF applied, 0 slope filter applied
      uint8_t LIR = 0x01; //0: interrupt request not latched(default); 1: interrupt request latched 
    };

    static uint8_t constexpr REGISTER_INTERRUPTS_EN            = 0x58;

    struct register_interrupt_en{
      uint8_t INTERRUPTS_ENABLE = 0x80; // Enables wake-up and activity/inactivity interrupt logic, 0 disabled
    };

    static uint8_t constexpr REGISTER_WAKE_UP_THS              = 0x5B;

    struct register_wake_up_this{
      uint8_t USR_OFF_ON_WU = 0x40; // Drives the low-pass filtered data with user offset correction to the wake-up function
      uint8_t 
    };

    static uint8_t constexpr REGISTER_WAKE_UP_DUR              = 0x5C;

    
    static uint8_t constexpr REGISTER_MD1_CFG                  = 0x5E;
    static uint8_t constexpr REGISTER_MD2_CFG                  = 0x5F;
    static uint8_t constexpr REGISTER_INTERNAL_FREQ_FINE       = 0x63;
    static uint8_t constexpr REGISTER_X_OFS_USR                = 0x73;
    static uint8_t constexpr REGISTER_Y_OFS_USR                = 0x74;
    static uint8_t constexpr REGISTER_Z_OFS_USR                = 0x75;
    static uint8_t constexpr REGISTER_FIFO_DATA_OUT_TAG        = 0x78;
    static uint8_t constexpr REGISTER_FIFO_DATA_OUT_X_L        = 0x79;
    static uint8_t constexpr REGISTER_FIFO_DATA_OUT_X_H        = 0x7A;
    static uint8_t constexpr REGISTER_FIFO_DATA_OUT_Y_L        = 0x7B;
    static uint8_t constexpr REGISTER_FIFO_DATA_OUT_Y_H        = 0x7C;
    static uint8_t constexpr REGISTER_FIFO_DATA_OUT_Z_L        = 0x7D;
    static uint8_t constexpr REGISTER_FIFO_DATA_OUT_Z_H        = 0x7E;
    
    /*Constructor*/
    //Can declare more types of constructors for different settings    
    IIS3DWB_KD(SPIClass * const s, int const cs, bool spi); //constructor for SPI

    /*Basic Settings*/

    bool init();
    void reset();

  protected : 

    bool init(uint8_t const expectedValue);
    void mySensorSettings();
    void writeByte(uint8_t reg, uint8_t value);
    uint8_t readByte(uint8_t reg);
    void readBytes(uint8_t reg, uint8_t count, uint8_t * dest);

    SPIClass *const _spi = &SPI;
    int const csPin = 5; //Can be changed with constructor currently set according to VSPI
    bool useSPI = false;
    SPISettings mySPISettings;

};

#endif 