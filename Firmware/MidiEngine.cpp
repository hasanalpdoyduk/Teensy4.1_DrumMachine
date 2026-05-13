#include "MidiEngine.h"

IntervalTimer MidiEngine::_midiClockTimer;
volatile bool MidiEngine::_midiClockFlag = false;

// GM standard drum note mapping (General MIDI Level 1 Spec)
const uint8_t MidiEngine::DRUM_NOTES[NUM_INSTRUMENTS] = {
    36, // Kick (Bass Drum 1)
    38, // Snare (Acoustic Snare)
    42, // Hi-Hat Closed
    46, // Hi-Hat Open
    45, // Tom Low (Low Tom)
    47  // Tom Mid (Low-Mid Tom)
};

void MidiEngine::midiClockISR() {
    _midiClockFlag = true;
}

uint32_t MidiEngine::calcMidiClockPeriodUs() const {
    // 24 MIDI clock pulses per quarter note
    return 60000000UL / _currentBpm / 24UL;
}

void MidiEngine::begin() {
    // usbMIDI is initialised automatically by the Teensy core.
}

void MidiEngine::setBpm(uint32_t bpm) {
    if (bpm == _currentBpm) return;
    _currentBpm = bpm;
    if (_clockActive) {
        // Restart timer with the new period to stay in tempo.
        _midiClockTimer.end();
        _midiClockTimer.begin(midiClockISR, calcMidiClockPeriodUs());
    }
}

void MidiEngine::sendNoteOn(uint8_t instrumentIndex) {
    if (instrumentIndex >= NUM_INSTRUMENTS) return;
    uint8_t note = DRUM_NOTES[instrumentIndex];
    usbMIDI.sendNoteOn(note, NOTE_VELOCITY, DRUM_CHANNEL);

    _pendingOff[instrumentIndex].note   = note;
    _pendingOff[instrumentIndex].dueAt  = millis() + NOTE_OFF_DELAY;
    _pendingOff[instrumentIndex].active = true;
}

void MidiEngine::startTransport() {
    _midiClockFlag = false;
    _clockActive   = true;
    _midiClockTimer.begin(midiClockISR, calcMidiClockPeriodUs());
    usbMIDI.sendRealTime(MIDI_START);
}

void MidiEngine::stopTransport() {
    _midiClockTimer.end();
    _clockActive = false;
    usbMIDI.sendRealTime(MIDI_STOP);
}

void MidiEngine::update() {
    // Send one pending MIDI clock pulse per loop if the ISR fired.
    if (_midiClockFlag) {
        noInterrupts();
        _midiClockFlag = false;
        interrupts();
        usbMIDI.sendRealTime(MIDI_CLOCK);
    }

    // Send deferred Note-Offs.
    uint32_t now = millis();
    for (uint8_t i = 0; i < NUM_INSTRUMENTS; i++) {
        if (_pendingOff[i].active && now >= _pendingOff[i].dueAt) {
            usbMIDI.sendNoteOff(_pendingOff[i].note, 0, DRUM_CHANNEL);
            _pendingOff[i].active = false;
        }
    }

    // Drain incoming MIDI messages so the USB buffer doesn't stall.
    while (usbMIDI.read()) {}
}
