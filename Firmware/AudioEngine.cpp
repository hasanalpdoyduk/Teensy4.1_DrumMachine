#include "AudioEngine.h"

#include "AudioSampleKick.h"
#include "AudioSampleSnare.h"
#include "AudioSampleHatcl.h"
#include "AudioSampleHatop.h"
#include "AudioSampleTomlow.h"
#include "AudioSampleTommid.h"

// ─── Audio object graph ───────────────────────────────────────────────────────
//
// Voices:  kick, snare, hatclosed, hatopen → mix1/mix2 (stereo pair)
//          tomlow, tommid                  → mixer3/mixer4 (stereo pair)
//
// Submix:  mix1  ─► mixer5 (L master)
//          mixer3 ─►mixer5
//          mix2  ─► mixer6 (R master)
//          mixer4 ─►mixer6
//
// Filter:  mixer5 ─► filterL (LP/BP/HP outputs)
//          mixer6 ─► filterR
//          filterL outputs + dry ─► filterSelectL (4-input selector)
//          filterR outputs + dry ─► filterSelectR
//
// Output:  filterSelectL ─► i2s_out L
//          filterSelectR ─► i2s_out R

// Voices
AudioPlayMemory          kick;
AudioPlayMemory          snare;
AudioPlayMemory          hatclosed;
AudioPlayMemory          hatopen;
AudioPlayMemory          tomlow;
AudioPlayMemory          tommid;

// Submix
AudioMixer4              mix1;
AudioMixer4              mix2;
AudioMixer4              mixer3;
AudioMixer4              mixer4;
AudioMixer4              mixer5;
AudioMixer4              mixer6;

// Filter stage
AudioFilterStateVariable filterL;
AudioFilterStateVariable filterR;

// Filter output selectors: 0=LP, 1=BP, 2=HP, 3=dry
AudioMixer4              filterSelectL;
AudioMixer4              filterSelectR;

// Output
AudioOutputI2S           i2s_out;
AudioControlSGTL5000     sgtl5000_1;

// ─── Patch cords ─────────────────────────────────────────────────────────────
AudioConnection pc01(kick,       0, mix1,    0);
AudioConnection pc02(kick,       0, mix2,    0);
AudioConnection pc03(snare,      0, mix1,    1);
AudioConnection pc04(snare,      0, mix2,    1);
AudioConnection pc05(hatclosed,  0, mix1,    2);
AudioConnection pc06(hatclosed,  0, mix2,    2);
AudioConnection pc07(hatopen,    0, mix1,    3);
AudioConnection pc08(hatopen,    0, mix2,    3);
AudioConnection pc09(tomlow,     0, mixer3,  0);
AudioConnection pc10(tomlow,     0, mixer4,  0);
AudioConnection pc11(tommid,     0, mixer3,  1);
AudioConnection pc12(tommid,     0, mixer4,  1);
AudioConnection pc13(mix1,       0, mixer5,  0);
AudioConnection pc14(mix2,       0, mixer6,  0);
AudioConnection pc15(mixer3,     0, mixer5,  1);
AudioConnection pc16(mixer4,     0, mixer6,  1);

// mixer5 → filterL and filterSelectL dry input (3)
AudioConnection pc17(mixer5,     0, filterL,       0);
AudioConnection pc18(mixer5,     0, filterSelectL, 3);
// filterL outputs → selector
AudioConnection pc19(filterL,    0, filterSelectL, 0); // LP
AudioConnection pc20(filterL,    1, filterSelectL, 1); // BP
AudioConnection pc21(filterL,    2, filterSelectL, 2); // HP

// mixer6 → filterR and filterSelectR dry input (3)
AudioConnection pc22(mixer6,     0, filterR,       0);
AudioConnection pc23(mixer6,     0, filterSelectR, 3);
AudioConnection pc24(filterR,    0, filterSelectR, 0);
AudioConnection pc25(filterR,    1, filterSelectR, 1);
AudioConnection pc26(filterR,    2, filterSelectR, 2);

// Selector → I2S output
AudioConnection pc27(filterSelectL, 0, i2s_out, 0);
AudioConnection pc28(filterSelectR, 0, i2s_out, 1);

// ─── AudioEngine methods ──────────────────────────────────────────────────────

bool AudioEngine::begin() {
    AudioMemory(100);

    configureMixers();

    // Filter defaults: 2 kHz, Butterworth Q, bypass (dry)
    filterL.frequency(2000.0f);
    filterL.resonance(FILTER_Q_DEFAULT);
    filterR.frequency(2000.0f);
    filterR.resonance(FILTER_Q_DEFAULT);
    applyFilterType(FilterType::Off);

    sgtl5000_1.enable();
    sgtl5000_1.volume(masterVolume);

    return true;
}

void AudioEngine::configureMixers() {
    // Kick / snare / hats
    mix1.gain(0, 0.45f); mix2.gain(0, 0.45f);
    mix1.gain(1, 0.45f); mix2.gain(1, 0.45f);
    mix1.gain(2, 0.35f); mix2.gain(2, 0.35f);
    mix1.gain(3, 0.35f); mix2.gain(3, 0.35f);

    // Toms
    mixer3.gain(0, 0.50f); mixer4.gain(0, 0.50f);
    mixer3.gain(1, 0.50f); mixer4.gain(1, 0.50f);
    mixer3.gain(2, 0.00f); mixer4.gain(2, 0.00f);
    mixer3.gain(3, 0.00f); mixer4.gain(3, 0.00f);

    // Master L/R
    mixer5.gain(0, 0.60f); mixer6.gain(0, 0.60f);
    mixer5.gain(1, 0.60f); mixer6.gain(1, 0.60f);
    mixer5.gain(2, 0.00f); mixer6.gain(2, 0.00f);
    mixer5.gain(3, 0.00f); mixer6.gain(3, 0.00f);
}

void AudioEngine::update() {}

void AudioEngine::triggerInstrument(uint8_t instrumentIndex,
                                    const InstrumentManager& instrumentManager) {
    if (instrumentIndex >= NUM_INSTRUMENTS) return;

    // Per-instrument level applied as mixer gain at next trigger.
    // decay and pitch modulation can be added here once DSP effects are wired in.
    (void)instrumentManager.getDecay(instrumentIndex);
    (void)instrumentManager.getPitch(instrumentIndex);
    (void)instrumentManager.getLevel(instrumentIndex);

    switch (instrumentIndex) {
        case 0: kick.stop();      kick.play(AudioSampleKick);        break;
        case 1: snare.stop();     snare.play(AudioSampleSnare);      break;
        case 2: hatopen.stop();   hatclosed.stop();
                hatclosed.play(AudioSampleHatcl);                    break; // choke open
        case 3: hatclosed.stop(); hatopen.stop();
                hatopen.play(AudioSampleHatop);                      break; // choke closed
        case 4: tomlow.stop();    tomlow.play(AudioSampleTomlow);    break;
        case 5: tommid.stop();    tommid.play(AudioSampleTommid);    break;
        default: break;
    }
}

void AudioEngine::setMasterVolumeRaw(int rawValue) {
    float vol = constrain(static_cast<float>(rawValue) / 1023.0f, 0.05f, 1.0f);
    masterVolume = vol;
    sgtl5000_1.volume(vol);
}

float AudioEngine::getMasterVolume() const { return masterVolume; }

void AudioEngine::applyFilterType(FilterType type) {
    // Zero all selector inputs, then enable the chosen path.
    for (uint8_t i = 0; i < 4; i++) {
        filterSelectL.gain(i, 0.0f);
        filterSelectR.gain(i, 0.0f);
    }

    float q = FILTER_Q_DEFAULT;

    switch (type) {
        case FilterType::LowPass:
            filterSelectL.gain(0, 1.0f);
            filterSelectR.gain(0, 1.0f);
            break;
        case FilterType::BandPass:
            filterSelectL.gain(1, 1.0f);
            filterSelectR.gain(1, 1.0f);
            break;
        case FilterType::HighPass:
            filterSelectL.gain(2, 1.0f);
            filterSelectR.gain(2, 1.0f);
            break;
        case FilterType::ResoLP:
            filterSelectL.gain(0, 1.0f);
            filterSelectR.gain(0, 1.0f);
            q = FILTER_Q_RES;
            break;
        case FilterType::Off:
        default:
            filterSelectL.gain(3, 1.0f); // dry
            filterSelectR.gain(3, 1.0f);
            break;
    }

    filterL.resonance(q);
    filterR.resonance(q);
}

void AudioEngine::setFilterType(FilterType type) {
    activeFilter = type;
    applyFilterType(type);
}

void AudioEngine::setFilterFrequency(float freqHz) {
    // No-op while filter is bypassed to avoid unnecessary DSP work.
    if (activeFilter == FilterType::Off) return;
    filterL.frequency(freqHz);
    filterR.frequency(freqHz);
}
