#include <Arduino.h>
#include <Wire.h>

#include "Config.h"
#include "Pins.h"
#include "MCPManager.h"
#include "ButtonManager.h"
#include "PotManager.h"
#include "Sequencer.h"
#include "ClockEngine.h"
#include "ModeManager.h"
#include "InstrumentManager.h"
#include "AudioEngine.h"
#include "MidiEngine.h"
#include "StorageManager.h"

// ─── Global objects ───────────────────────────────────────────────────────────
MCPManager        mcpManager;
ButtonManager     buttonManager;
PotManager        potManager;
Sequencer         sequencer;
ClockEngine       clockEngine;
ModeManager       modeManager;
InstrumentManager instrumentManager;
AudioEngine       audioEngine;
MidiEngine        midiEngine;
StorageManager    storageManager;

PlayMode previousMode = PlayMode::LivePlay;

// ─── Shared trigger helper (audio + MIDI in one call) ─────────────────────────
void triggerInstrument(uint8_t inst) {
    audioEngine.triggerInstrument(inst, instrumentManager);
    midiEngine.sendNoteOn(inst);
}

// ─── Debug helpers ────────────────────────────────────────────────────────────
void printModeChangeIfNeeded() {
    PlayMode currentMode = modeManager.getMode();
    if (currentMode != previousMode) {
        DEBUG_PRINT("Mode: ");
        DEBUG_PRINTLN(modeManager.getModeName());
        previousMode = currentMode;
    }
}

#if DEBUG_ENABLED
void printPotDebugOccasionally() {
    static uint32_t lastPrint = 0;
    if (millis() - lastPrint < 500) return;
    lastPrint = millis();

    uint8_t t = instrumentManager.getEditTargetInstrument(modeManager, sequencer);
    DEBUG_PRINTF("BPM:%d  Sel:%d  Last:%d  Edit:%d  D:%d  P:%d  L:%d  Vol:%d\n",
                 potManager.getBpm(),
                 sequencer.getSelectedInstrument(),
                 instrumentManager.getLastPlayedInstrument(), t,
                 instrumentManager.getDecay(t),
                 instrumentManager.getPitch(t),
                 instrumentManager.getLevel(t),
                 potManager.getRawMasterVolume());
}
#endif

// ─── Input handlers ───────────────────────────────────────────────────────────
void handleModeSpecificInstrumentButtons() {
    for (uint8_t i = 0; i < NUM_INSTRUMENTS; i++) {
        if (!buttonManager.wasInstrumentJustPressed(i)) continue;

        if (modeManager.isLivePlay()) {
            instrumentManager.setLastPlayedInstrument(i);
            sequencer.setSelectedInstrument(i);
            triggerInstrument(i);
            DEBUG_PRINTF("[LIVE] inst %u\n", i);
        }
        else if (modeManager.isSeqEdit()) {
            sequencer.setSelectedInstrument(i);
            DEBUG_PRINTF("[EDIT] select inst %u\n", i);
        }
        else if (modeManager.isSeqPlay()) {
            instrumentManager.setLastPlayedInstrument(i);
            sequencer.setSelectedInstrument(i);
            triggerInstrument(i);

            if (clockEngine.isRunning()) {
                uint8_t step = sequencer.getCurrentStep();
                if (!sequencer.isStepActive(i, step)) {
                    sequencer.toggleStep(i, step);
                    DEBUG_PRINTF("[REC] inst %u step %u\n", i, step);
                }
            }
        }
    }
}

void handleStepButtons() {
    if (!(modeManager.isSeqEdit() || modeManager.isSeqPlay())) return;

    uint8_t sel = sequencer.getSelectedInstrument();
    for (uint8_t step = 0; step < NUM_STEPS; step++) {
        if (buttonManager.wasStepJustPressed(step)) {
            sequencer.toggleStep(sel, step);
            DEBUG_PRINTF("[STEP] inst %u step %u = %d\n",
                         sel, step, sequencer.isStepActive(sel, step));
        }
    }
}

void handleTransportButtons() {
    if (buttonManager.wasPlayJustPressed()) {
        if (modeManager.isSeqEdit() || modeManager.isSeqPlay()) {
            clockEngine.start();
            midiEngine.startTransport();
            DEBUG_PRINTLN("PLAY");
        }
    }

    if (buttonManager.wasStopJustPressed()) {
        if (modeManager.isSeqEdit() || modeManager.isSeqPlay()) {
            clockEngine.stop();
            midiEngine.stopTransport();
            sequencer.resetStep();
            DEBUG_PRINTLN("STOP");
        }
    }
}

void handleClock() {
    const uint32_t bpm = (uint32_t)potManager.getBpm();
    clockEngine.setBpm(bpm);
    midiEngine.setBpm(bpm);
    clockEngine.update(); // no-op with IntervalTimer — kept for readability

    if (!clockEngine.shouldAdvanceStep()) return;

    sequencer.advanceStep();
    const uint8_t step = sequencer.getCurrentStep();
    DEBUG_PRINTF("step %u\n", step);

    if (modeManager.isSeqEdit() || modeManager.isSeqPlay()) {
        for (uint8_t inst = 0; inst < NUM_INSTRUMENTS; inst++) {
            if (sequencer.isStepActive(inst, step)) {
                instrumentManager.setLastPlayedInstrument(inst);
                triggerInstrument(inst);
            }
        }
    }
}

// ─── Filter ───────────────────────────────────────────────────────────────────
void handleFilterButtons() {
    for (uint8_t i = 0; i < NUM_FILTER_BUTTONS; i++) {
        if (buttonManager.wasFilterButtonJustPressed(i)) {
            audioEngine.setFilterType(static_cast<FilterType>(i));
            DEBUG_PRINTF("[FILTER] type %u\n", i);
        }
    }
}

// ─── Pattern save (hold STOP for SAVE_HOLD_MS while sequencer is stopped) ─────
void flashSaveFeedback() {
    // All step LEDs blink twice as save confirmation.
    // IntervalTimer continues during delay(), so audio is unaffected.
    for (uint8_t i = 0; i < NUM_STEPS; i++) mcpManager.setStepLed(i, true);
    mcpManager.writeAllStepLeds();
    delay(150);
    for (uint8_t i = 0; i < NUM_STEPS; i++) mcpManager.setStepLed(i, false);
    mcpManager.writeAllStepLeds();
    delay(80);
    for (uint8_t i = 0; i < NUM_STEPS; i++) mcpManager.setStepLed(i, true);
    mcpManager.writeAllStepLeds();
    delay(150);
}

void handleSaveLongPress() {
    static uint32_t holdStart      = 0;
    static bool     savedThisPress = false;

    if (buttonManager.isStopPressed()) {
        if (holdStart == 0) { holdStart = millis(); savedThisPress = false; }
        if (!savedThisPress && millis() - holdStart >= SAVE_HOLD_MS) {
            storageManager.save(sequencer, (uint32_t)potManager.getBpm());
            savedThisPress = true;
            flashSaveFeedback();
            DEBUG_PRINTLN("Pattern saved to EEPROM");
        }
    } else {
        holdStart = 0;
    }
}

// ─── LED display ──────────────────────────────────────────────────────────────
void updateStepLeds() {
    const uint8_t sel     = sequencer.getSelectedInstrument();
    const uint8_t curStep = sequencer.getCurrentStep();
    const bool    running = clockEngine.isRunning();
    const bool    seqMode = modeManager.isSeqEdit() || modeManager.isSeqPlay();

    for (uint8_t step = 0; step < NUM_STEPS; step++) {
        bool on = seqMode && (sequencer.isStepActive(sel, step) ||
                              (running && step == curStep));
        mcpManager.setStepLed(step, on);
    }
    mcpManager.writeAllStepLeds();
}

// ─── Setup ────────────────────────────────────────────────────────────────────
void setup() {
#if DEBUG_ENABLED
    Serial.begin(115200);
    delay(500);
#endif

    Wire2.begin();
    mcpManager.begin(Wire2);
    buttonManager.begin(&mcpManager);
    potManager.begin();
    sequencer.begin();
    clockEngine.begin(DEFAULT_BPM);
    modeManager.begin();
    instrumentManager.begin();
    midiEngine.begin();
    storageManager.begin();

    if (!audioEngine.begin()) {
        DEBUG_PRINTLN("Audio engine init failed!");
    }

    // Auto-load last saved pattern from EEPROM.
    {
        uint32_t savedBpm = DEFAULT_BPM;
        if (storageManager.load(sequencer, savedBpm)) {
            clockEngine.begin(savedBpm);
            DEBUG_PRINTLN("Pattern loaded from EEPROM");
        }
    }

    previousMode = modeManager.getMode();
    DEBUG_PRINTLN("Drum Machine ready");
}

// ─── Main loop ────────────────────────────────────────────────────────────────
void loop() {
    buttonManager.update();
    potManager.update();
    modeManager.update(buttonManager);
    instrumentManager.updateFromPots(modeManager, sequencer, potManager);

    audioEngine.setMasterVolumeRaw(potManager.getRawMasterVolume());
    audioEngine.setFilterFrequency(potManager.getFilterFreqHz());
    audioEngine.update();

    midiEngine.update();

    printModeChangeIfNeeded();

    handleModeSpecificInstrumentButtons();
    handleStepButtons();
    handleTransportButtons();
    handleClock();
    handleFilterButtons();
    handleSaveLongPress();
    updateStepLeds();

#if DEBUG_ENABLED
    printPotDebugOccasionally();
#endif
}
