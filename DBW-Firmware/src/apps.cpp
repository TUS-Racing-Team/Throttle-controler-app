#include "apps.h"

#include "pins.h"
#include "config.h"
#include "read_data.h"

#include <Arduino.h>
#include <math.h>

int app1Min = 50;
int app1Max = 3200;
int app2Min = 50;
int app2Max = 3200;

static constexpr int RAW_MARGIN = 80;
static constexpr int ADC_SHORT_LOW = 5;
static constexpr int ADC_SHORT_HIGH = 4090;
static constexpr float SENSOR_MISMATCH_LIMIT = 10.0f;

static float clampf(float x, float lo, float hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

static bool rawInRange(int raw, int mn, int mx, int margin) {
    if (raw <= ADC_SHORT_LOW || raw >= ADC_SHORT_HIGH) {
        return false;
    }

    const int lo = (mn < mx) ? mn : mx;
    const int hi = (mn < mx) ? mx : mn;

    return raw >= (lo - margin) && raw <= (hi + margin);
}

static float toPct(int raw, int mn, int mx) {
    if (mx == mn) return 0.0f;

    float v = (float)(raw - mn) / (float)(mx - mn);
    return clampf(v, 0.0f, 1.0f) * 100.0f;
}

ReadData readAppsPct() {
    const int raw1 = analogRead(PIN_APPS1);
    const int raw2 = analogRead(PIN_APPS2);

    const bool rawOk =
        rawInRange(raw1, app1Min, app1Max, RAW_MARGIN) &&
        rawInRange(raw2, app2Min, app2Max, RAW_MARGIN);

    const float p1 = toPct(raw1, app1Min, app1Max);
    const float p2 = toPct(raw2, app2Min, app2Max);

    const bool correlationOk = fabsf(p1 - p2) <= SENSOR_MISMATCH_LIMIT;
    const bool valid = rawOk && correlationOk;

    return ReadData{(p1 + p2) * 0.5f, valid};
}
