#ifndef MIDIENGINE_H
#define MIDIENGINE_H

#include <Arduino.h>
#include <IntervalTimer.h>
#include "Config.h"

// Requires Arduino IDE: Tools > USB Type = "Serial + MIDI" (Teensyduino).
// Provides:
//   • Note On/Off output on GM drum channel (ch 10) for each instrument trigger
//   • MIDI Start / Stop transport messages
//   • 24-PPQ MIDI clock output driven by a hardware IntervalTimer
class MidiEngine {
public:
    void begin();

    // Call every main loop to flush pending Note-Offs and MIDI clock pulses.
    void update();

    void setBpm(uint32_t bpm);

    void sendNoteOn(uint8_t instrumentIndex);

    void startTransport();
    void stopTransport();

private:
    struct PendingNoteOff {
        uint8_t  note;
        uint32_t dueAt; // millis() when Note-Off should be sent
        bool     active;
    };

    PendingNoteOff _pendingOff[NUM_INSTRUMENTS] = {};
    uint32_t       _currentBpm  = 120;
    bool           _clockActive = false;

    // 24-PPQ hardware clock
    static IntervalTimer _midiClockTimer;
    static volatile bool _midiClockFlag;
    static void          midiClockISR();
    uint32_t             calcMidiClockPeriodUs() const;

    // GM standard drum note numbers for each instrument slot
    static const uint8_t DRUM_NOTES[NUM_INSTRUMENTS];

    static constexpr uint8_t DRUM_CHANNEL    = 10;  // GM drums (1-indexed)
    static constexpr uint8_t NOTE_VELOCITY   = 100;
    static constexpr uint8_t NOTE_OFF_DELAY  = 20;  // ms

    // MIDI real-time status bytes
    static constexpr uint8_t MIDI_CLOCK = 0xF8;
    static constexpr uint8_t MIDI_START = 0xFA;
    static constexpr uint8_t MIDI_STOP  = 0xFC;
};

#endif
