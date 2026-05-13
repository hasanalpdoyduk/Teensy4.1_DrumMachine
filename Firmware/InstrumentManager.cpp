#include "InstrumentManager.h"

void InstrumentManager::begin() {
    lastPlayedInstrument = 0;

    for (uint8_t i = 0; i < NUM_INSTRUMENTS; i++) {
        instruments[i].decay = 0;
        instruments[i].pitch = 0;
        instruments[i].level = 0;
    }
}

void InstrumentManager::setLastPlayedInstrument(uint8_t instrument) {
    if (instrument < NUM_INSTRUMENTS) {
        lastPlayedInstrument = instrument;
    }
}

uint8_t InstrumentManager::getLastPlayedInstrument() const {
    return lastPlayedInstrument;
}

uint8_t InstrumentManager::getEditTargetInstrument(const ModeManager& modeManager,
                                                   const Sequencer& sequencer) const {
    if (modeManager.isSeqEdit()) {
        return sequencer.getSelectedInstrument();
    }

    // LivePlay ve SeqPlay
    return lastPlayedInstrument;
}

void InstrumentManager::updateFromPots(const ModeManager& modeManager,
                                       const Sequencer& sequencer,
                                       const PotManager& potManager) {
    uint8_t target = getEditTargetInstrument(modeManager, sequencer);

    if (target >= NUM_INSTRUMENTS) return;

    instruments[target].decay = potManager.getRawDecay();
    instruments[target].pitch = potManager.getRawPitch();
    instruments[target].level = potManager.getRawLevel();
}

const InstrumentParams& InstrumentManager::getParams(uint8_t instrument) const {
    static InstrumentParams dummy;
    if (instrument >= NUM_INSTRUMENTS) return dummy;
    return instruments[instrument];
}

int InstrumentManager::getDecay(uint8_t instrument) const {
    return (instrument < NUM_INSTRUMENTS) ? instruments[instrument].decay : 0;
}

int InstrumentManager::getPitch(uint8_t instrument) const {
    return (instrument < NUM_INSTRUMENTS) ? instruments[instrument].pitch : 0;
}

int InstrumentManager::getLevel(uint8_t instrument) const {
    return (instrument < NUM_INSTRUMENTS) ? instruments[instrument].level : 0;
}