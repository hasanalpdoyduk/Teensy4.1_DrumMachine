#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

// I2C bus (Teensy 4.1 - Wire2)
constexpr uint8_t PIN_I2C2_SCL = 24;
constexpr uint8_t PIN_I2C2_SDA = 25;

// Analog potlar
constexpr uint8_t PIN_FILTER_POT        = 26;
constexpr uint8_t PIN_MASTER_VOLUME_POT = 27;
constexpr uint8_t PIN_DECAY_POT         = 38;
constexpr uint8_t PIN_PITCH_POT         = 39;
constexpr uint8_t PIN_LEVEL_POT         = 40;
constexpr uint8_t PIN_BPM_POT           = 41;

// Dijital butonlar
constexpr uint8_t PIN_PLAY_BUTTON  = 28;
constexpr uint8_t PIN_STOP_BUTTON  = 29;
constexpr uint8_t PIN_RIGHT_BUTTON = 30;
constexpr uint8_t PIN_LEFT_BUTTON  = 31;

// 32 boş

constexpr uint8_t PIN_FILTER_BTN_1 = 33;
constexpr uint8_t PIN_FILTER_BTN_2 = 34;
constexpr uint8_t PIN_FILTER_BTN_3 = 35;
constexpr uint8_t PIN_FILTER_BTN_4 = 36;
constexpr uint8_t PIN_FILTER_BTN_5 = 37;

#endif