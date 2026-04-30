#include "dac.h"

#include <Arduino.h>

static const float DAC_MIN_PCT = 0.0f;
static const float DAC_MAX_PCT = 100.0f;
static const uint16_t DAC_MIN = 0;
static const uint16_t DAC_MAX = 4095;
static const float DAC_IDLE_PCT = 7.5f;

static inline float clampf(float x, float a, float b) {
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

void dacInit() {
    analogWriteResolution(12);
    analogWrite(DAC0, DAC_MIN);
}

void dacSetRaw(uint16_t value) {
    if (value > DAC_MAX) value = DAC_MAX;
    analogWrite(DAC0, value);
}

void dacSetThrottlePct(float pct) {
    pct = clampf(pct, DAC_IDLE_PCT, DAC_MAX_PCT);

    const float norm = pct / 100.0f;
    const uint16_t dacVal = (uint16_t)(norm * (float)DAC_MAX);

    dacSetRaw(dacVal);
}
