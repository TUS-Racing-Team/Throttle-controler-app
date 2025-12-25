#include "tps.h"
#include "adc.h"
#include "pins.h"
#include "config.h"

int tps1Min = 1070;
int tps1Max = 3390;
int tps2Min = 375;
int tps2Max = 3340;

static float clampf(float x, float a, float b) {
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

static float toPct(int raw, int mn, int mx) {
    float v = (float)(raw - mn) / (float)(mx - mn);
    return clampf(v, 0.0f, 1.0f) * 100.0f;
}

float readThrottlePct() {
    float p1 = toPct(readADC(PIN_TPS1), tps1Min, tps1Max);
    float p2 = toPct(readADC(PIN_TPS2), tps2Min, tps2Max);
    return (p1 + p2) * 0.5f;
}
