#include "apps.h"
#include "adc.h"
#include "pins.h"
#include "config.h"
#include "read_data.h"
#include <Arduino.h>

// TODO: callibrate these values
int app1Min = 1070;
int app1Max = 3390;
int app2Min = 375;
int app2Max = 3340;


static float clampf(float x, float a, float b) {
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

static float toPct(int raw, int mn, int mx) {
    float v = (float)(raw - mn) / (float)(mx - mn);
    return clampf(v, 0.0f, 1.0f) * 100.0f;
}

ReadData readAppsPct() {
    float p1 = toPct(analogRead(PIN_APPS1), app1Min, app1Max);
    float p2 = toPct(analogRead(PIN_APPS2), app2Min, app2Max);
    Serial.print("APPS1: "); Serial.print(p1);
    Serial.print(" APPS2: "); Serial.println(p2);
    if (fabsf(p1 - p2) > 10.0f) {
        // Sensor mismatch
        return ReadData{(p1 + p2) * 0.5f, false};
    }
    return ReadData{(p1 + p2) * 0.5f, true};
}
