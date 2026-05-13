#include "Sequencer.h"

void Sequencer::begin() {
    selectedInstrument = 0;
    currentStep        = 0;
    clearPattern();
}

void Sequencer::clearPattern() {
    for (uint8_t inst = 0; inst < NUM_INSTRUMENTS; inst++)
        for (uint8_t step = 0; step < NUM_STEPS; step++)
            pattern[inst][step] = false;
}

void Sequencer::toggleStep(uint8_t instrument, uint8_t step) {
    if (instrument >= NUM_INSTRUMENTS || step >= NUM_STEPS) return;
    pattern[instrument][step] = !pattern[instrument][step];
}

void Sequencer::setStep(uint8_t instrument, uint8_t step, bool active) {
    if (instrument >= NUM_INSTRUMENTS || step >= NUM_STEPS) return;
    pattern[instrument][step] = active;
}

bool Sequencer::isStepActive(uint8_t instrument, uint8_t step) const {
    if (instrument >= NUM_INSTRUMENTS || step >= NUM_STEPS) return false;
    return pattern[instrument][step];
}

void Sequencer::setSelectedInstrument(uint8_t instrument) {
    if (instrument < NUM_INSTRUMENTS) selectedInstrument = instrument;
}

uint8_t Sequencer::getSelectedInstrument() const { return selectedInstrument; }

void Sequencer::advanceStep() {
    currentStep++;
    if (currentStep >= NUM_STEPS) currentStep = 0;
}

void Sequencer::resetStep() { currentStep = 0; }

uint8_t Sequencer::getCurrentStep() const { return currentStep; }
