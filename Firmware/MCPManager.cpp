#include "MCPManager.h"

// MCP23017 register adresleri
static constexpr uint8_t IODIRA = 0x00;
static constexpr uint8_t IODIRB = 0x01;
static constexpr uint8_t GPPUA  = 0x0C;
static constexpr uint8_t GPPUB  = 0x0D;
static constexpr uint8_t GPIOA  = 0x12;
static constexpr uint8_t GPIOB  = 0x13;
static constexpr uint8_t OLATA  = 0x14;
static constexpr uint8_t OLATB  = 0x15;

bool MCPManager::begin(TwoWire& wirePort) {
    wire = &wirePort;
    configureMCPs();
    return true;
}

void MCPManager::configureMCPs() {
    // MCP1: GPA input (step buttons), GPB output (step leds)
    writeRegister(MCP1_ADDR, IODIRA, 0xFF);
    writeRegister(MCP1_ADDR, IODIRB, 0x00);
    writeRegister(MCP1_ADDR, GPPUA, 0xFF); // internal pullup
    writeRegister(MCP1_ADDR, OLATB, 0x00);

    // MCP2: GPA input (step buttons), GPB output (step leds)
    writeRegister(MCP2_ADDR, IODIRA, 0xFF);
    writeRegister(MCP2_ADDR, IODIRB, 0x00);
    writeRegister(MCP2_ADDR, GPPUA, 0xFF);
    writeRegister(MCP2_ADDR, OLATB, 0x00);

    // MCP3: GPA input (mode), GPB input (instrument + metronome)
    writeRegister(MCP3_ADDR, IODIRA, 0xFF);
    writeRegister(MCP3_ADDR, IODIRB, 0xFF);
    writeRegister(MCP3_ADDR, GPPUA, 0xFF);
    writeRegister(MCP3_ADDR, GPPUB, 0xFF);
}

void MCPManager::update() {
    // şimdilik boş
}

bool MCPManager::readStepButton(uint8_t stepIndex) {
    uint8_t gpio = 0xFF;

    if (stepIndex < 8) {
        if (!readRegister(MCP1_ADDR, GPIOA, gpio)) return false;
        return !(gpio & (1 << stepIndex)); // aktif low
    } else {
        if (!readRegister(MCP2_ADDR, GPIOA, gpio)) return false;
        return !(gpio & (1 << (stepIndex - 8)));
    }
}

bool MCPManager::readInstrumentButton(uint8_t instrumentIndex) {
    // instrumentIndex: 0..5 => GPB1..GPB6
    if (instrumentIndex >= NUM_INSTRUMENTS) return false;

    uint8_t gpio = 0xFF;
    if (!readRegister(MCP3_ADDR, GPIOB, gpio)) return false;

    uint8_t bitIndex = instrumentIndex + 1;
    return !(gpio & (1 << bitIndex)); // aktif low
}

bool MCPManager::readMetronomeButton() {
    uint8_t gpio = 0xFF;
    if (!readRegister(MCP3_ADDR, GPIOB, gpio)) return false;

    return !(gpio & (1 << 7)); // GPB7
}

bool MCPManager::readModeButton(uint8_t modeIndex) {
    if (modeIndex >= NUM_MODE_BUTTONS) return false;

    uint8_t gpio = 0xFF;
    if (!readRegister(MCP3_ADDR, GPIOA, gpio)) return false;

    return !(gpio & (1 << modeIndex)); // GPA0,1,2
}

void MCPManager::setStepLed(uint8_t stepIndex, bool on) {
    if (stepIndex < 8) {
        // step0 -> GPB7, step1 -> GPB6 ... step7 -> GPB0
        uint8_t bitPos = 7 - stepIndex;
        if (on) stepLedState1 |=  (1 << bitPos);
        else    stepLedState1 &= ~(1 << bitPos);
    } else if (stepIndex < 16) {
        uint8_t bitPos = 15 - stepIndex; // 8->7, 15->0
        if (on) stepLedState2 |=  (1 << bitPos);
        else    stepLedState2 &= ~(1 << bitPos);
    }
}

void MCPManager::writeAllStepLeds() {
    writeRegister(MCP1_ADDR, OLATB, stepLedState1);
    writeRegister(MCP2_ADDR, OLATB, stepLedState2);
}

bool MCPManager::writeRegister(uint8_t addr, uint8_t reg, uint8_t value) {
    wire->beginTransmission(addr);
    wire->write(reg);
    wire->write(value);
    return (wire->endTransmission() == 0);
}

bool MCPManager::readRegister(uint8_t addr, uint8_t reg, uint8_t& value) {
    wire->beginTransmission(addr);
    wire->write(reg);
    if (wire->endTransmission(false) != 0) return false;

    if (wire->requestFrom((int)addr, 1) != 1) return false;
    value = wire->read();
    return true;
}