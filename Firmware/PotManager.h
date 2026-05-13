#ifndef POTMANAGER_H
#define POTMANAGER_H

#include <Arduino.h>
#include "Pins.h"
#include "Config.h"

class PotManager {
public:
    void begin();
    void update();

    int   getBpm()            const;
    float getFilterFreqHz()   const; // exponential map → [FILTER_FREQ_MIN, FILTER_FREQ_MAX]
    int   getRawFilter()      const;
    int   getRawMasterVolume()const;
    int   getRawDecay()       const;
    int   getRawPitch()       const;
    int   getRawLevel()       const;

private:
    int filterValue       = 0;
    int masterVolumeValue = 0;
    int decayValue        = 0;
    int pitchValue        = 0;
    int levelValue        = 0;
    int bpmRawValue       = 0;

    int smoothRead(uint8_t pin, int previous);
};

#endif
