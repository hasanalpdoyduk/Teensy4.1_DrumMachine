#ifndef INSTRUMENTMANAGER_H
#define INSTRUMENTMANAGER_H

#include <Arduino.h>
#include "Config.h"
#include "ModeManager.h"
#include "Sequencer.h"
#include "PotManager.h"

struct InstrumentParams {
    int decay = 0;
    int pitch = 0;
    int level = 0;
};

class InstrumentManager {
public:
    void begin();

    void setLastPlayedInstrument(uint8_t instrument);
    uint8_t getLastPlayedInstrument() const;

    uint8_t getEditTargetInstrument(const ModeManager& modeManager,
                                    const Sequencer& sequencer) const;

    void updateFromPots(const ModeManager& modeManager,
                        const Sequencer& sequencer,
                        const PotManager& potManager);

    const InstrumentParams& getParams(uint8_t instrument) const;

    int getDecay(uint8_t instrument) const;
    int getPitch(uint8_t instrument) const;
    int getLevel(uint8_t instrument) const;

private:
    InstrumentParams instruments[NUM_INSTRUMENTS];
    uint8_t lastPlayedInstrument = 0;
};

#endif