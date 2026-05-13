#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <Arduino.h>
#include <Audio.h>

#include "Config.h"
#include "InstrumentManager.h"

enum class FilterType : uint8_t {
    Off      = 0, // dry bypass
    LowPass  = 1, // classic cutoff sweep
    BandPass = 2, // mid-range resonance
    HighPass = 3, // cut bass
    ResoLP   = 4  // low-pass with high Q — wub / synth bass effect
};

class AudioEngine {
public:
    bool begin();
    void update();

    void triggerInstrument(uint8_t instrumentIndex,
                           const InstrumentManager& instrumentManager);

    void setMasterVolumeRaw(int rawValue);
    float getMasterVolume() const;

    // Called once when a filter button is pressed.
    void setFilterType(FilterType type);

    // Called every main-loop iteration with the current pot frequency.
    void setFilterFrequency(float freqHz);

private:
    float      masterVolume   = 0.6f;
    FilterType activeFilter   = FilterType::Off;

    void configureMixers();
    void applyFilterType(FilterType type);
};

#endif
