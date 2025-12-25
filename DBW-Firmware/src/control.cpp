#include "control.h"
#include "motor.h"
#include "tps.h"
#include "config.h"
#include <math.h>

float alpha = 0.20f;
float posF = 0;

const float IDLE_POS = 7.5f;
const float FAR_ZONE = 10.0f;
const float DEADBAND = 1.0f;

const int PWM_MIN = 40;
const int PWM_NEAR_MAX = 90;
const int PWM_FAR = 110;

void controlInit() {
    posF = 0;
}

void controlUpdate(int cmdPct) {
    float target = (cmdPct < (int)ceil(IDLE_POS)) ? IDLE_POS : (float)cmdPct;

    float pos = readThrottlePct();
    posF += alpha * (pos - posF);

    float err = target - posF;
    float eAbs = fabsf(err);

    if (eAbs <= DEADBAND) {
        // motorStop();
        return;
    }

    int pwm;
    if (eAbs > FAR_ZONE) {
        pwm = PWM_FAR;
    } else {
        float t = eAbs / FAR_ZONE;
        pwm = (int)(PWM_MIN + t * (PWM_NEAR_MAX - PWM_MIN));
    }

    if (err > 0) motorOpen(pwm);
    else motorClose(pwm);
}
