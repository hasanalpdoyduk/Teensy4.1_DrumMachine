#include "ClockEngine.h"

IntervalTimer    ClockEngine::_timer;
volatile bool    ClockEngine::_stepFlag = false;

void ClockEngine::timerISR() {
    _stepFlag = true;
}

uint32_t ClockEngine::calcPeriodUs() const {
    // 16th-note period in microseconds: 60 000 000 / BPM / 4 steps-per-quarter
    return 60000000UL / currentBpm / 4UL;
}

void ClockEngine::begin(uint32_t bpm) {
    currentBpm = bpm;
    running    = false;
    _stepFlag  = false;
}

void ClockEngine::setBpm(uint32_t bpm) {
    if (bpm == currentBpm) return;
    currentBpm = bpm;
    if (running) {
        _timer.end();
        _timer.begin(timerISR, calcPeriodUs());
    }
}

void ClockEngine::start() {
    _stepFlag = false;
    running   = true;
    _timer.begin(timerISR, calcPeriodUs());
}

void ClockEngine::stop() {
    _timer.end();
    running   = false;
    _stepFlag = false;
}

bool ClockEngine::isRunning() const {
    return running;
}

void ClockEngine::update() {
    // Timing is interrupt-driven; this method is a no-op kept for API compatibility.
}

bool ClockEngine::shouldAdvanceStep() {
    if (!_stepFlag) return false;
    noInterrupts();
    _stepFlag = false;
    interrupts();
    return true;
}
