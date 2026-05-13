#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ─── Debug ───────────────────────────────────────────────────────────────────
// 0 = production build (no serial output)
// 1 = enable serial debug output
#define DEBUG_ENABLED 0

#if DEBUG_ENABLED
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
  #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(...)
#endif

// ─── System ──────────────────────────────────────────────────────────────────
constexpr uint8_t NUM_STEPS          = 16;
constexpr uint8_t NUM_INSTRUMENTS    = 6;
constexpr uint8_t NUM_MODE_BUTTONS   = 3;
constexpr uint8_t NUM_FILTER_BUTTONS = 5;

// ─── MCP23017 I2C addresses ───────────────────────────────────────────────────
constexpr uint8_t MCP1_ADDR = 0x20;
constexpr uint8_t MCP2_ADDR = 0x21;
constexpr uint8_t MCP3_ADDR = 0x22;

// ─── Timing ──────────────────────────────────────────────────────────────────
constexpr uint32_t DEFAULT_BPM        = 120;
constexpr uint16_t BUTTON_DEBOUNCE_MS = 25;

// ─── Potentiometer ───────────────────────────────────────────────────────────
constexpr uint8_t POT_SMOOTHING = 8;

// ─── BPM range ───────────────────────────────────────────────────────────────
constexpr uint32_t BPM_MIN = 60;
constexpr uint32_t BPM_MAX = 180;

// ─── Filter ──────────────────────────────────────────────────────────────────
constexpr float FILTER_FREQ_MIN  =   200.0f; // Hz — pot minimum
constexpr float FILTER_FREQ_MAX  = 18000.0f; // Hz — pot maximum
constexpr float FILTER_Q_DEFAULT =   0.707f; // Butterworth (LP / HP)
constexpr float FILTER_Q_RES     =   3.5f;   // High-resonance sweep mode

// ─── Storage ─────────────────────────────────────────────────────────────────
constexpr uint32_t SAVE_HOLD_MS = 2000; // hold STOP this long to save

#endif
