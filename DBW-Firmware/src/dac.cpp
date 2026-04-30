#include "dac.h"

#include "config.h"

#include <Arduino.h>

static const float DAC_MAX_PCT = 100.0f;
static const uint16_t DAC_MAX = 4095;
static const uint16_t DAC_INVALID_RAW = 0xFFFFU;

static uint16_t lastDacRaw = DAC_INVALID_RAW;

static inline float clampf(float x, float a, float b) {
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

void dacInit() {
    lastDacRaw = DAC_INVALID_RAW;
    dacSetThrottlePct(ECU_DAC_IDLE_PCT);
}

void dacSetRaw(uint16_t value) {
    if (value > DAC_MAX) value = DAC_MAX;
    if (value == lastDacRaw) return;

    // Arduino Due uses one global analogWrite resolution for DAC and PWM.
    // Write DAC as 12-bit, then restore 8-bit so motor PWM stays correct.
    analogWriteResolution(12);
    analogWrite(DAC0, value);
    analogWriteResolution(8);

    lastDacRaw = value;
}

void dacSetThrottlePct(float pct) {
    pct = clampf(pct, ECU_DAC_IDLE_PCT, DAC_MAX_PCT);

    const float rawMin = clampf(ECU_DAC_MIN_RAW, 0.0f, (float)DAC_MAX);
    const float rawMax = clampf(ECU_DAC_MAX_RAW, rawMin, (float)DAC_MAX);

    const float norm = pct / 100.0f;
    const uint16_t dacVal = (uint16_t)(rawMin + norm * (rawMax - rawMin));

    dacSetRaw(dacVal);
}
