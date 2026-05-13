#include "PotManager.h"

void PotManager::begin() {}

int PotManager::smoothRead(uint8_t pin, int previous) {
    int raw = analogRead(pin);
    return (previous * (POT_SMOOTHING - 1) + raw) / POT_SMOOTHING;
}

void PotManager::update() {
    filterValue       = smoothRead(PIN_FILTER_POT,        filterValue);
    masterVolumeValue = smoothRead(PIN_MASTER_VOLUME_POT, masterVolumeValue);
    decayValue        = smoothRead(PIN_DECAY_POT,         decayValue);
    pitchValue        = smoothRead(PIN_PITCH_POT,         pitchValue);
    levelValue        = smoothRead(PIN_LEVEL_POT,         levelValue);
    bpmRawValue       = smoothRead(PIN_BPM_POT,           bpmRawValue);
}

int PotManager::getBpm() const {
    return map(bpmRawValue, 0, 1023, BPM_MIN, BPM_MAX);
}

float PotManager::getFilterFreqHz() const {
    // Exponential mapping gives a more musical (octave-linear) sweep.
    // t=0 → FILTER_FREQ_MIN, t=1 → FILTER_FREQ_MAX
    float t = static_cast<float>(filterValue) / 1023.0f;
    return FILTER_FREQ_MIN * powf(FILTER_FREQ_MAX / FILTER_FREQ_MIN, t);
}

int PotManager::getRawFilter()       const { return filterValue; }
int PotManager::getRawMasterVolume() const { return masterVolumeValue; }
int PotManager::getRawDecay()        const { return decayValue; }
int PotManager::getRawPitch()        const { return pitchValue; }
int PotManager::getRawLevel()        const { return levelValue; }
