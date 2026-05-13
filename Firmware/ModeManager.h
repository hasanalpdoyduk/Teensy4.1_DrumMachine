#ifndef MODEMANAGER_H
#define MODEMANAGER_H

#include <Arduino.h>
#include "ButtonManager.h"

enum class PlayMode {
    SeqEdit = 0,
    SeqPlay = 1,
    LivePlay = 2
};

class ModeManager {
public:
    void begin();
    void update(ButtonManager& buttons);

    PlayMode getMode() const;
    const char* getModeName() const;

    bool isSeqEdit() const;
    bool isSeqPlay() const;
    bool isLivePlay() const;

private:
    PlayMode currentMode = PlayMode::LivePlay;
};

#endif