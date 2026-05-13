#ifndef CLOCKENGINE_H
#define CLOCKENGINE_H

#include <Arduino.h>
#include <IntervalTimer.h>

// Uses a Teensy 4.1 PIT hardware timer for microsecond-precision step timing.
// No drift from integer millisecond rounding; step interval is recalculated
// exactly when BPM changes.
class ClockEngine {
public:
    void begin(uint32_t bpm);
    void update();           // no-op — kept so the main loop compiles unchanged
    void setBpm(uint32_t bpm);

    void start();
    void stop();
    bool isRunning() const;

    // Returns true exactly once per step tick; clears the ISR flag atomically.
    bool shouldAdvanceStep();

private:
    uint32_t currentBpm = 120;
    bool     running    = false;

    static IntervalTimer _timer;
    static volatile bool _stepFlag;

    static void timerISR();
    uint32_t    calcPeriodUs() const;
};

#endif
