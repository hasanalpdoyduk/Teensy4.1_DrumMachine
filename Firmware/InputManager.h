#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <Arduino.h>
#include "Config.h"
#include "Pins.h"
#include "MCPManager.h"

class InputManager {
public:
    void begin(MCPManager* mcpRef);
    void update();

    bool isStepPressed(uint8_t stepIndex) const;
    bool wasStepJustPressed(uint8_t stepIndex) const;

    bool wasPlayJustPressed() const;
    bool wasStopJustPressed() const;
    bool wasLeftJustPressed() const;
    bool wasRightJustPressed() const;

    bool wasInstrumentJustPressed(uint8_t instrumentIndex) const;
    bool wasModeJustPressed(uint8_t modeIndex) const;
    bool wasMetronomeJustPressed() const;

private:
    MCPManager* mcp = nullptr;

    bool stepState[NUM_STEPS] = {false};
    bool stepPrev[NUM_STEPS]  = {false};

    bool instrumentState[NUM_INSTRUMENTS] = {false};
    bool instrumentPrev[NUM_INSTRUMENTS]  = {false};

    bool modeState[NUM_MODE_BUTTONS] = {false};
    bool modePrev[NUM_MODE_BUTTONS]  = {false};

    bool metroState = false;
    bool metroPrev  = false;

    bool playState = false, playPrev = false;
    bool stopState = false, stopPrev = false;
    bool leftState = false, leftPrev = false;
    bool rightState = false, rightPrev = false;

    bool readDirectButton(uint8_t pin);
};

#endif