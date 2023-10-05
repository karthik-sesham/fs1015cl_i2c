//Library to enable I2C communication between Arduino
//and FS1015CL mass flow sensor

#ifndef _FS1015_H_
#define _FS1015_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <Wire.h>
/*----------------------------------------*/

//Hex Commands for FS1015

#define FS1015_DEFAULT_ADDR           0x01

//WRITE COMMANDS
#define FS1015_WRITE_ADDR             0x05  //Bit 0 is R/W bit, Bits 1 ~ 7 are address
#define FS1015_WRITE_FILTER_DEPTH     0x0B  //Int8, 0 ~ 254
#define FS1015_CALIBRATE_OFFSET       0x1C  //1 byte, ensure no flow conditions
#define FS1015_WRITE_PROTECTION       0x9D  //4 bytes, 0x53, 0x49, 0x41, 0x52. One time effective

//READ COMMANDS
#define FS1015_READ_SERIAL            0x82  //ASCII
#define FS1015_READ_FLOWRATE          0x83  //Int32 (/1000 SLPM) + CRC. see datasheet for crc info
#define FS1015_READ_ADDR              0x85  //Bit 7 ~ 1
#define FS1015_READ_FILTER_DEPTH      0x8B  //Int8 0 ~ 254

static inline uint8_t i2cread(void);

static inline void i2cwrite(uint8_t x);

class fs1015{
  public:

    fs1015(bool debug);
    bool begin(uint8_t addr);
    void write_addr(uint8_t addr);
    void write_filter_depth(uint8_t depth);
    void calibrate();
    std::string read_sn();
    double read_flowrate();
    uint8_t read_addr();
    uint8_t read_filter_depth();

    private:

    int8_t read_register(uint8_t reg);
    void write_register(uint8_t reg, int8_t value);

    uint8_t _i2caddr;
    bool _debug; //set to 1 if you want to see debug output
};

#endif