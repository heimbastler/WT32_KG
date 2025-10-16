#ifndef _ADAFRUIT_PCA9535_H_
#define _ADAFRUIT_PCA9535_H_

#include <Wire.h>

class Adafruit_PCA9535 {
public:
    Adafruit_PCA9535();
    bool begin(uint8_t addr = 0x20, TwoWire *theWire = &Wire);
    void pinMode(uint8_t pin, uint8_t mode);
    void digitalWrite(uint8_t pin, uint8_t value);
    uint8_t digitalRead(uint8_t pin);

private:
    TwoWire *_wire;
    uint8_t _i2caddr;
    uint16_t _outputState;
    uint16_t _configState;
    
    void writeRegister(uint8_t reg, uint16_t value);
    uint16_t readRegister(uint8_t reg);
};

#endif