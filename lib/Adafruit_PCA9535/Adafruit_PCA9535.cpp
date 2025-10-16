#include "Adafruit_PCA9535.h"

// PCA9535 Register addresses
#define PCA9535_INPUT_PORT0_REG    0x00
#define PCA9535_INPUT_PORT1_REG    0x01
#define PCA9535_OUTPUT_PORT0_REG   0x02
#define PCA9535_OUTPUT_PORT1_REG   0x03
#define PCA9535_POLARITY_PORT0_REG 0x04
#define PCA9535_POLARITY_PORT1_REG 0x05
#define PCA9535_CONFIG_PORT0_REG   0x06
#define PCA9535_CONFIG_PORT1_REG   0x07

Adafruit_PCA9535::Adafruit_PCA9535() {
    _wire = &Wire;
    _i2caddr = 0x20;
    _outputState = 0;
    _configState = 0xFFFF; // All pins as inputs by default
}

bool Adafruit_PCA9535::begin(uint8_t addr, TwoWire *theWire) {
    _i2caddr = addr;
    _wire = theWire;
    
    // Test if device is present
    _wire->beginTransmission(_i2caddr);
    if (_wire->endTransmission() != 0) {
        return false;
    }
    
    // Initialize all pins as inputs with high output state
    writeRegister(PCA9535_CONFIG_PORT0_REG, 0xFFFF);
    writeRegister(PCA9535_OUTPUT_PORT0_REG, 0x0000);
    
    return true;
}

void Adafruit_PCA9535::pinMode(uint8_t pin, uint8_t mode) {
    if (pin >= 16) return;
    
    if (mode == INPUT) {
        _configState |= (1 << pin);
    } else {
        _configState &= ~(1 << pin);
    }
    
    writeRegister(PCA9535_CONFIG_PORT0_REG, _configState);
}

void Adafruit_PCA9535::digitalWrite(uint8_t pin, uint8_t value) {
    if (pin >= 16) return;
    
    if (value == HIGH) {
        _outputState |= (1 << pin);
    } else {
        _outputState &= ~(1 << pin);
    }
    
    writeRegister(PCA9535_OUTPUT_PORT0_REG, _outputState);
}

uint8_t Adafruit_PCA9535::digitalRead(uint8_t pin) {
    if (pin >= 16) return LOW;
    
    uint16_t inputState = readRegister(PCA9535_INPUT_PORT0_REG);
    return (inputState & (1 << pin)) ? HIGH : LOW;
}

void Adafruit_PCA9535::writeRegister(uint8_t reg, uint16_t value) {
    _wire->beginTransmission(_i2caddr);
    _wire->write(reg);
    _wire->write(value & 0xFF);        // Low byte
    _wire->write((value >> 8) & 0xFF); // High byte
    _wire->endTransmission();
}

uint16_t Adafruit_PCA9535::readRegister(uint8_t reg) {
    _wire->beginTransmission(_i2caddr);
    _wire->write(reg);
    _wire->endTransmission();
    
    _wire->requestFrom(_i2caddr, (uint8_t)2);
    uint16_t value = _wire->read();        // Low byte
    value |= ((uint16_t)_wire->read()) << 8; // High byte
    
    return value;
}