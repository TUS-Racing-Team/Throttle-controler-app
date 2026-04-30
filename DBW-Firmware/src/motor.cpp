#include "motor.h"

#include "pins.h"

#include <Arduino.h>

static constexpr int PWM_UNKNOWN = -1;

static int lastRPwm = PWM_UNKNOWN;
static int lastLPwm = PWM_UNKNOWN;
static bool driverEnabled = false;
static bool driverStateKnown = false;

static int clampPwm(int pwm) {
    if (pwm < 0) return 0;
    if (pwm > 255) return 255;
    return pwm;
}

static void writePwmIfChanged(int pin, int& lastValue, int value) {
    if (lastValue == value) {
        return;
    }

    analogWrite(pin, value);
    lastValue = value;
}

static void setDriverEnabled(bool enabled) {
    if (driverStateKnown && driverEnabled == enabled) {
        return;
    }

    digitalWrite(PIN_REN, enabled ? HIGH : LOW);
    digitalWrite(PIN_LEN, enabled ? HIGH : LOW);

    driverEnabled = enabled;
    driverStateKnown = true;
}

void motorInit() {
    pinMode(PIN_RPWM, OUTPUT);
    pinMode(PIN_LPWM, OUTPUT);
    pinMode(PIN_REN, OUTPUT);
    pinMode(PIN_LEN, OUTPUT);

    lastRPwm = PWM_UNKNOWN;
    lastLPwm = PWM_UNKNOWN;
    driverStateKnown = false;

    motorDisable();
}

void motorEnable() {
    setDriverEnabled(true);
}

void motorDisable() {
    writePwmIfChanged(PIN_RPWM, lastRPwm, 0);
    writePwmIfChanged(PIN_LPWM, lastLPwm, 0);

    setDriverEnabled(false);
}

void motorOpen(int pwm) {
    pwm = clampPwm(pwm);

    motorEnable();

    writePwmIfChanged(PIN_LPWM, lastLPwm, 0);
    writePwmIfChanged(PIN_RPWM, lastRPwm, pwm);
}

void motorClose(int pwm) {
    pwm = clampPwm(pwm);

    motorEnable();

    writePwmIfChanged(PIN_RPWM, lastRPwm, 0);
    writePwmIfChanged(PIN_LPWM, lastLPwm, pwm);
}

void motorStop() {
    writePwmIfChanged(PIN_RPWM, lastRPwm, 0);
    writePwmIfChanged(PIN_LPWM, lastLPwm, 0);
}
