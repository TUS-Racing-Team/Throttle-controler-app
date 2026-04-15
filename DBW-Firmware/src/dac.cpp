#include "dac.h"
#include <Arduino.h>

// ===== НАСТРОЙКИ =====
static const float DAC_MIN_PCT = 0.0f;     // ECU вижда това като 0%
static const float DAC_MAX_PCT = 100.0f;   // ECU вижда това като 100%

static const uint16_t DAC_MIN = 0;          // 0 V
static const uint16_t DAC_MAX = 4095;       // 3.3 V

static const float DAC_IDLE_PCT = 7.5f;

// ===== INTERNAL =====
static inline float clampf(float x, float a, float b) {
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

void dacInit() {
    // DAC0 е на пин DAC0
    analogWriteResolution(12);
    analogWrite(DAC0, 0);   // старт от 0
}

void dacSetRaw(uint16_t value) {
    if (value > DAC_MAX) value = DAC_MAX;
    analogWrite(DAC0, value);
}

void dacSetThrottlePct(float pct) {
    // clamp + idle
    pct = clampf(pct, DAC_IDLE_PCT, DAC_MAX_PCT);

    // % → DAC value
    float norm = pct / 100.0f;
    uint16_t dacVal = (uint16_t)(norm * (float)DAC_MAX);

    dacSetRaw(dacVal);
}
