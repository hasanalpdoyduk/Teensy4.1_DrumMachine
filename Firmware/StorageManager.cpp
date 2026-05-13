#include "StorageManager.h"

void StorageManager::begin() {
    // Teensy emulated EEPROM requires no explicit initialisation.
}

bool StorageManager::hasSavedData() const {
    uint16_t magic = 0;
    EEPROM.get(SAVE_ADDR, magic);
    return magic == SAVE_MAGIC;
}

StorageManager::SaveData StorageManager::packData(const Sequencer& sequencer,
                                                   uint32_t bpm) const {
    SaveData data;
    data.magic = SAVE_MAGIC;
    data.bpm   = static_cast<uint16_t>(constrain((long)bpm, BPM_MIN, BPM_MAX));

    for (uint8_t inst = 0; inst < NUM_INSTRUMENTS; inst++) {
        data.pattern[inst] = 0;
        for (uint8_t step = 0; step < NUM_STEPS; step++) {
            if (sequencer.isStepActive(inst, step))
                data.pattern[inst] |= (1u << step);
        }
    }
    return data;
}

void StorageManager::unpackData(const SaveData& data,
                                 Sequencer& sequencer,
                                 uint32_t& bpm) const {
    bpm = constrain(data.bpm, (uint16_t)BPM_MIN, (uint16_t)BPM_MAX);
    sequencer.clearPattern();
    for (uint8_t inst = 0; inst < NUM_INSTRUMENTS; inst++)
        for (uint8_t step = 0; step < NUM_STEPS; step++)
            sequencer.setStep(inst, step, (data.pattern[inst] >> step) & 0x01);
}

void StorageManager::save(const Sequencer& sequencer, uint32_t bpm) {
    SaveData data = packData(sequencer, bpm);
    EEPROM.put(SAVE_ADDR, data);
}

bool StorageManager::load(Sequencer& sequencer, uint32_t& bpm) {
    SaveData data;
    EEPROM.get(SAVE_ADDR, data);
    if (data.magic != SAVE_MAGIC) return false;
    unpackData(data, sequencer, bpm);
    return true;
}
