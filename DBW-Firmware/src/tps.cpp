#include "tps.h"
#include "adc.h"
#include "pins.h"
#include "config.h"
#include "read_data.h"

int tps1Min = 880;
int tps1Max = 2720;
int tps2Min = 300;
int tps2Max = 2600;


static float clampf(float x, float a, float b) {
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

static float toPct(int raw, int mn, int mx) {
    float v = (float)(raw - mn) / (float)(mx - mn);
    return clampf(v, 0.0f, 1.0f) * 100.0f;
}

ReadData readThrottlePct() {
    float p1 = toPct(analogRead(PIN_TPS1), tps1Min, tps1Max);
    float p2 = toPct(analogRead(PIN_TPS2), tps2Min, tps2Max);
    // Serial.print("TPS1: "); Serial.print(p1);
    // Serial.print(" TPS2: "); Serial.println(p2);
    if (fabsf(p1 - p2) > 10.0f) {
        // Sensor mismatch
        return ReadData{(p1 + p2) * 0.5f, false};
    }
    return ReadData{(p1 + p2) * 0.5f, true};
}
