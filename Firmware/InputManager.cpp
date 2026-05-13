#include "InputManager.h"

void InputManager::begin(MCPManager* mcpRef) {
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

bool InputManager::readDirectButton(uint8_t pin) {
    return digitalRead(pin) == LOW; // buton GND'ye çekiyor
}

void InputManager::update() {
    for (uint8_t i = 0; i < NUM_STEPS; i++) {
        stepPrev[i] = stepState[i];
        stepState[i] = mcp->readStepButton(i);
    }

    for (uint8_t i = 0; i < NUM_INSTRUMENTS; i++) {
        instrumentPrev[i] = instrumentState[i];
        instrumentState[i] = mcp->readInstrumentButton(i);
    }

    for (uint8_t i = 0; i < NUM_MODE_BUTTONS; i++) {
        modePrev[i] = modeState[i];
        modeState[i] = mcp->readModeButton(i);
    }

    metroPrev = metroState;
    metroState = mcp->readMetronomeButton();

    playPrev = playState;
    stopPrev = stopState;
    leftPrev = leftState;
    rightPrev = rightState;

    playState = readDirectButton(PIN_PLAY_BUTTON);
    stopState = readDirectButton(PIN_STOP_BUTTON);
    leftState = readDirectButton(PIN_LEFT_BUTTON);
    rightState = readDirectButton(PIN_RIGHT_BUTTON);
}

bool InputManager::isStepPressed(uint8_t stepIndex) const {
    return (stepIndex < NUM_STEPS) ? stepState[stepIndex] : false;
}

bool InputManager::wasStepJustPressed(uint8_t stepIndex) const {
    return (stepIndex < NUM_STEPS) ? (stepState[stepIndex] && !stepPrev[stepIndex]) : false;
}

bool InputManager::wasPlayJustPressed() const {
    return playState && !playPrev;
}

bool InputManager::wasStopJustPressed() const {
    return stopState && !stopPrev;
}

bool InputManager::wasLeftJustPressed() const {
    return leftState && !leftPrev;
}

bool InputManager::wasRightJustPressed() const {
    return rightState && !rightPrev;
}

bool InputManager::wasInstrumentJustPressed(uint8_t instrumentIndex) const {
    return (instrumentIndex < NUM_INSTRUMENTS)
        ? (instrumentState[instrumentIndex] && !instrumentPrev[instrumentIndex])
        : false;
}

bool InputManager::wasModeJustPressed(uint8_t modeIndex) const {
    return (modeIndex < NUM_MODE_BUTTONS)
        ? (modeState[modeIndex] && !modePrev[modeIndex])
        : false;
}

bool InputManager::wasMetronomeJustPressed() const {
    return metroState && !metroPrev;
}