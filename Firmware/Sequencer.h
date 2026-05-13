#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <Arduino.h>
#include "Config.h"

class Sequencer {
public:
    void begin();

    void toggleStep(uint8_t instrument, uint8_t step);
    bool isStepActive(uint8_t instrument, uint8_t step) const;

    // Direct write — used by StorageManager when loading a saved pattern.
    void setStep(uint8_t instrument, uint8_t step, bool active);
    void clearPattern();

    void setSelectedInstrument(uint8_t instrument);
    uint8_t getSelectedInstrument() const;

    void advanceStep();
    void resetStep();
    uint8_t getCurrentStep() const;

private:
    bool    pattern[NUM_INSTRUMENTS][NUM_STEPS] = {};
    uint8_t selectedInstrument = 0;
    uint8_t currentStep        = 0;
};

#endif
