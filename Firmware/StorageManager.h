#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <Arduino.h>
#include <EEPROM.h>
#include "Config.h"
#include "Sequencer.h"

// Persists drum patterns and BPM to Teensy 4.1 emulated EEPROM (flash-backed).
// Total footprint: 16 bytes.
//
// Layout at address 0:
//   [0..1]   magic  (uint16_t) — 0xABCD if valid data exists
//   [2..3]   bpm    (uint16_t)
//   [4..15]  pattern (uint16_t × 6) — one bitmask per instrument, bit N = step N
class StorageManager {
public:
    void begin();

    // Returns true if EEPROM contains previously saved data.
    bool hasSavedData() const;

    // Writes current pattern and BPM to EEPROM.
    void save(const Sequencer& sequencer, uint32_t bpm);

    // Reads pattern and BPM from EEPROM.
    // Returns false if no valid save data is found; sequencer is unchanged.
    bool load(Sequencer& sequencer, uint32_t& bpm);

private:
    struct SaveData {
        uint16_t magic;
        uint16_t bpm;
        uint16_t pattern[NUM_INSTRUMENTS]; // bitmask: bit N set → step N active
    };

    static constexpr uint16_t SAVE_MAGIC = 0xABCD;
    static constexpr int      SAVE_ADDR  = 0;

    SaveData packData(const Sequencer& sequencer, uint32_t bpm) const;
    void     unpackData(const SaveData& data, Sequencer& sequencer, uint32_t& bpm) const;
};

#endif
