#ifndef BUTTONMANAGER_H
#define BUTTONMANAGER_H

#include <Arduino.h>
#include "Config.h"
#include "Pins.h"
#include "MCPManager.h"

struct ButtonState {
    bool rawState = false;
    bool stableState = false;
    bool lastStableState = false;
    bool justPressed = false;
    bool justReleased = false;
    uint32_t lastChangeTime = 0;
};

class ButtonManager {
public:
    void begin(MCPManager* mcpRef);
    void update();

    bool isStepPressed(uint8_t stepIndex) const;
    bool wasStepJustPressed(uint8_t stepIndex) const;
    bool wasStepJustReleased(uint8_t stepIndex) const;

    bool isInstrumentPressed(uint8_t instrumentIndex) const;
    bool wasInstrumentJustPressed(uint8_t instrumentIndex) const;
    bool wasInstrumentJustReleased(uint8_t instrumentIndex) const;

    bool isModePressed(uint8_t modeIndex) const;
    bool wasModeJustPressed(uint8_t modeIndex) const;

    bool isMetronomePressed() const;
    bool wasMetronomeJustPressed() const;

    bool isPlayPressed() const;
    bool wasPlayJustPressed() const;

    bool isStopPressed() const;
    bool wasStopJustPressed() const;

    bool isLeftPressed() const;
    bool wasLeftJustPressed() const;

    bool isRightPressed() const;
    bool wasRightJustPressed() const;

    bool isFilterButtonPressed(uint8_t index) const;
    bool wasFilterButtonJustPressed(uint8_t index) const;

private:
    MCPManager* mcp = nullptr;

    ButtonState stepButtons[NUM_STEPS];
    ButtonState instrumentButtons[NUM_INSTRUMENTS];
    ButtonState modeButtons[NUM_MODE_BUTTONS];
    ButtonState metronomeButton;

    ButtonState playButton;
    ButtonState stopButton;
    ButtonState leftButton;
    ButtonState rightButton;

    static constexpr uint8_t NUM_FILTER_BUTTONS = 5;
    ButtonState filterButtons[NUM_FILTER_BUTTONS];

    void updateButton(ButtonState& button, bool rawReading);
    bool readDirectButton(uint8_t pin) const;
};

#endif