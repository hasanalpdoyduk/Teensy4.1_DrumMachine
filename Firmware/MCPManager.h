#ifndef MCPMANAGER_H
#define MCPMANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include "Config.h"

class MCPManager {
public:
    bool begin(TwoWire& wirePort);

    void update();

    bool readStepButton(uint8_t stepIndex);
    bool readInstrumentButton(uint8_t instrumentIndex);
    bool readMetronomeButton();
    bool readModeButton(uint8_t modeIndex);

    void setStepLed(uint8_t stepIndex, bool on);
    void writeAllStepLeds();

private:
    TwoWire* wire = nullptr;

    uint8_t stepLedState1 = 0; // step 1-8 için GPB
    uint8_t stepLedState2 = 0; // step 9-16 için GPB

    bool writeRegister(uint8_t addr, uint8_t reg, uint8_t value);
    bool readRegister(uint8_t addr, uint8_t reg, uint8_t& value);

    void configureMCPs();
};

#endif