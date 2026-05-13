#include "ModeManager.h"

void ModeManager::begin() {
    currentMode = PlayMode::LivePlay;
}

void ModeManager::update(ButtonManager& buttons) {
    if (buttons.wasModeJustPressed(0)) {
        currentMode = PlayMode::SeqEdit;
    }
    else if (buttons.wasModeJustPressed(1)) {
        currentMode = PlayMode::SeqPlay;
    }
    else if (buttons.wasModeJustPressed(2)) {
        currentMode = PlayMode::LivePlay;
    }
}

PlayMode ModeManager::getMode() const {
    return currentMode;
}

const char* ModeManager::getModeName() const {
    switch (currentMode) {
        case PlayMode::SeqEdit:  return "SeqEdit";
        case PlayMode::SeqPlay:  return "SeqPlay";
        case PlayMode::LivePlay: return "LivePlay";
        default:                 return "Unknown";
    }
}

bool ModeManager::isSeqEdit() const {
    return currentMode == PlayMode::SeqEdit;
}

bool ModeManager::isSeqPlay() const {
    return currentMode == PlayMode::SeqPlay;
}

bool ModeManager::isLivePlay() const {
    return currentMode == PlayMode::LivePlay;
}