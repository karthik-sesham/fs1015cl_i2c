//Library to enable I2C communication between Arduino
//and FS1015CL mass flow sensor

#include "fs1015.h"

static inline uint8_t i2cread(void) {
  #if ARDUINO >= 100
  return Wire.read();
  #else
  return Wire.receive();
  #endif
}

static inline void i2cwrite(uint8_t x) {
  #if ARDUINO >= 100
  Wire.write((uint8_t)x);
  #else
  Wire.send(x);
  #endif
}

fs1015::fs1015(bool debug = false) : _i2caddr(FS1015_DEFAULT_ADDR), _debug(debug){}

//Starts the sensor with default address 1
bool fs1015::begin(uint8_t addr = FS4001_DEFAULT_ADDR){
    Wire.begin(4,5); //CHANGE TO 21, 22 if using pins 21 and 22 for SDA and SCL

    Serial.print("Started new sensor w/ addr: "); Serial.println(addr);

    calibrate();
    write_filter_depth(0xFE); //Set filter depth to 254

    return true;
}

//Allows user to change the address of the device
void fs1015::write_addr(uint8_t addr){
    Serial.print("Writing addr as "); Serial.print(addr); 
    if(_debug) Serial.print(": ");
    else Serial.println();

    write_register(FS4001_WRITE_ADDR, addr); //CHANGE BACK IF NEEDED
    _i2caddr = addr; 
}

//Writes the filter depth for the sensor
void fs1015::write_filter_depth(uint8_t depth){
    Serial.print("Writing filter depth as "); Serial.print(depth);
    if(_debug) Serial.print(": ");
    else Serial.println();

    write_register(FS1015_WRITE_FILTER_DEPTH, depth);
}

//Calibrates the sensor. Ensure no flow in the sensor
void fs1015::calibrate(){
    Serial.print("Calibrating offset");
    if(_debug) Serial.print(": ");
    else Serial.println();

    write_register(FS1015_CALIBRATE_OFFSET, 0x00);
}

//Reads 1 byte from device
int8_t fs1015::read_register(uint8_t reg){
    Wire.beginTransmission(_i2caddr);
    i2cwrite(reg);

    int temp = Wire.endTransmission(false); //repeated start sequence
    int temp1 = Wire.requestFrom(_i2caddr, 1); //requests 1 bytes from device register

    //for debugging
    if(_debug){
        Serial.print("\tread reg code: "); Serial.print(temp);
        Serial.print("\tread bytes available: "); Serial.println(temp1); 
    }

    if (! Wire.available()){
        Serial.println("\tERROR in Register read"); //for debugging
        return -1;
    }

    //Read the register and return
    int8_t result = i2cread();
    return result;
}

//Writes 1 bytes of data to device
void fs1015::write_register(uint8_t reg, int8_t value){
    Wire.beginTransmission(_i2caddr);
    i2cwrite(reg);

    //Sends the data
    i2cwrite(value);
    int temp = Wire.endTransmission();

    //for debugging
    if(_debug) Serial.print("\twrite reg code: "); Serial.print(temp); Serial.println();
}

//Reads serial number
//Note: Add .c_str() at the end of this function if 
//you want to print it to serial in the Arduino IDE
std::string fs1015::read_sn(){
    Serial.print("Reading serial number: ");

    Wire.beginTransmission(_i2caddr);
    i2cwrite(FS1015_READ_SERIAL);

    int temp = Wire.endTransmission(false); 
    int temp1 = Wire.requestFrom(_i2caddr, 12);

    //for debugging
    if(_debug){
        Serial.print("\tread reg code: "); Serial.print(temp);
        Serial.print("\tread bytes available: "); Serial.println(temp1); 
    }
    
    if (! Wire.available()){
        Serial.println("\tERROR in Register read"); //for debugging
        return "";
    }

    std::string result = "";
    for (int i = 0; i < 12; ++i) result += i2cread();

    return result;
}

//Returns flowrate in sccm
double fs1015::read_flowrate(){
    Serial.print("Reading flowrate: ");

    Wire.beginTransmission(_i2caddr);
    i2cwrite(FS1015_READ_FLOWRATE);
    int temp = Wire.endTransmission(false); 
    int temp1 = Wire.requestFrom(_i2caddr, 5);

    //for debugging
    if(_debug){
    Serial.print("\tread reg code: "); Serial.print(temp);
    Serial.print("\tread bytes available: "); Serial.println(temp1);
    }
    
    int64_t flowrate = i2cread(); flowrate <<= 8;
    flowrate |= i2cread(); flowrate <<= 8;
    flowrate |= i2cread(); flowrate <<= 8;
    flowrate |= i2cread(); flowrate <<= 2; //might not need 2 shift??

    
    uint8_t crc = i2cread();  //crc needs to be processed in a different way

    if(_debug) Serial.print("Crc: "); Serial.println(crc);

    return flowrate / 1000.0;
}

//Reads the address of the sensor
uint8_t fs1015::read_addr(){
    Serial.print("Reading addr: ");
    uint8_t addr = read_register(FS1015_READ_ADDR) >> 1;
    return addr;
}

//Reads the filter depth of the sensor
uint8_t fs1015::read_filter_depth(){
    Serial.print("Reading filter depth: "); 
    return read_register(FS1015_READ_FILTER_DEPTH);
}
