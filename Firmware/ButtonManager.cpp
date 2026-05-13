#include "ButtonManager.h"

void ButtonManager::begin(MCPManager* mcpRef) {
    mcp = mcpRef;

    pinMode(PIN_PLAY_BUTTON, INPUT_PULLUP);
    pinMode(PIN_STOP_BUTTON, INPUT_PULLUP);
    pinMode(PIN_LEFT_BUTTON, INPUT_PULLUP);
    pinMode(PIN_RIGHT_BUTTON, INPUT_PULLUP);

    pinMode(PIN_FILTER_BTN_1, INPUT_PULLUP);
    pinMode(PIN_FILTER_BTN_2, INPUT_PULLUP);
    pinMode(PIN_FILTER_BTN_3, INPUT_PULLUP);
    pinMode(PIN_FILTER_BTN_4, INPUT_PULLUP);
    pinMode(PIN_FILTER_BTN_5, INPUT_PULLUP);
}

bool ButtonManager::readDirectButton(uint8_t pin) const {
    return digitalRead(pin) == LOW; // active low
}

void ButtonManager::updateButton(ButtonState& button, bool rawReading) {
    uint32_t now = millis();

    button.justPressed = false;
    button.justReleased = false;

    if (rawReading != button.rawState) {
        button.rawState = rawReading;
        button.lastChangeTime = now;
    }

    if ((now - button.lastChangeTime) >= BUTTON_DEBOUNCE_MS) {
        if (button.stableState != button.rawState) {
            button.lastStableState = button.stableState;
            button.stableState = button.rawState;

            if (button.stableState && !button.lastStableState) {
                button.justPressed = true;
            } else if (!button.stableState && button.lastStableState) {
                button.justReleased = true;
            }
        }
    }
}

void ButtonManager::update() {
    for (uint8_t i = 0; i < NUM_STEPS; i++) {
        updateButton(stepButtons[i], mcp->readStepButton(i));
    }

    for (uint8_t i = 0; i < NUM_INSTRUMENTS; i++) {
        updateButton(instrumentButtons[i], mcp->readInstrumentButton(i));
    }

    for (uint8_t i = 0; i < NUM_MODE_BUTTONS; i++) {
        updateButton(modeButtons[i], mcp->readModeButton(i));
    }

    updateButton(metronomeButton, mcp->readMetronomeButton());

    updateButton(playButton,  readDirectButton(PIN_PLAY_BUTTON));
    updateButton(stopButton,  readDirectButton(PIN_STOP_BUTTON));
    updateButton(leftButton,  readDirectButton(PIN_LEFT_BUTTON));
    updateButton(rightButton, readDirectButton(PIN_RIGHT_BUTTON));

    updateButton(filterButtons[0], readDirectButton(PIN_FILTER_BTN_1));
    updateButton(filterButtons[1], readDirectButton(PIN_FILTER_BTN_2));
    updateButton(filterButtons[2], readDirectButton(PIN_FILTER_BTN_3));
    updateButton(filterButtons[3], readDirectButton(PIN_FILTER_BTN_4));
    updateButton(filterButtons[4], readDirectButton(PIN_FILTER_BTN_5));
}

bool ButtonManager::isStepPressed(uint8_t stepIndex) const {
    return (stepIndex < NUM_STEPS) ? stepButtons[stepIndex].stableState : false;
}

bool ButtonManager::wasStepJustPressed(uint8_t stepIndex) const {
    return (stepIndex < NUM_STEPS) ? stepButtons[stepIndex].justPressed : false;
}

bool ButtonManager::wasStepJustReleased(uint8_t stepIndex) const {
    return (stepIndex < NUM_STEPS) ? stepButtons[stepIndex].justReleased : false;
}

bool ButtonManager::isInstrumentPressed(uint8_t instrumentIndex) const {
    return (instrumentIndex < NUM_INSTRUMENTS) ? instrumentButtons[instrumentIndex].stableState : false;
}

bool ButtonManager::wasInstrumentJustPressed(uint8_t instrumentIndex) const {
    return (instrumentIndex < NUM_INSTRUMENTS) ? instrumentButtons[instrumentIndex].justPressed : false;
}

bool ButtonManager::wasInstrumentJustReleased(uint8_t instrumentIndex) const {
    return (instrumentIndex < NUM_INSTRUMENTS) ? instrumentButtons[instrumentIndex].justReleased : false;
}

bool ButtonManager::isModePressed(uint8_t modeIndex) const {
    return (modeIndex < NUM_MODE_BUTTONS) ? modeButtons[modeIndex].stableState : false;
}

bool ButtonManager::wasModeJustPressed(uint8_t modeIndex) const {
    return (modeIndex < NUM_MODE_BUTTONS) ? modeButtons[modeIndex].justPressed : false;
}

bool ButtonManager::isMetronomePressed() const {
    return metronomeButton.stableState;
}

bool ButtonManager::wasMetronomeJustPressed() const {
    return metronomeButton.justPressed;
}

bool ButtonManager::isPlayPressed() const {
    return playButton.stableState;
}

bool ButtonManager::wasPlayJustPressed() const {
    return playButton.justPressed;
}

bool ButtonManager::isStopPressed() const {
    return stopButton.stableState;
}

bool ButtonManager::wasStopJustPressed() const {
    return stopButton.justPressed;
}

bool ButtonManager::isLeftPressed() const {
    return leftButton.stableState;
}

bool ButtonManager::wasLeftJustPressed() const {
    return leftButton.justPressed;
}

bool ButtonManager::isRightPressed() const {
    return rightButton.stableState;
}

bool ButtonManager::wasRightJustPressed() const {
    return rightButton.justPressed;
}

bool ButtonManager::isFilterButtonPressed(uint8_t index) const {
    return (index < NUM_FILTER_BUTTONS) ? filterButtons[index].stableState : false;
}

bool ButtonManager::wasFilterButtonJustPressed(uint8_t index) const {
    return (index < NUM_FILTER_BUTTONS) ? filterButtons[index].justPressed : false;
}