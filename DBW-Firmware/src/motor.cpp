#include "motor.h"

#include "pins.h"

#include <Arduino.h>

static int clampPwm(int pwm) {
    if (pwm < 0) return 0;
    if (pwm > 255) return 255;
    return pwm;
}

void motorInit() {
    pinMode(PIN_RPWM, OUTPUT);
    pinMode(PIN_LPWM, OUTPUT);
    pinMode(PIN_REN, OUTPUT);
    pinMode(PIN_LEN, OUTPUT);

    motorDisable();
}

void motorEnable() {
    digitalWrite(PIN_REN, HIGH);
    digitalWrite(PIN_LEN, HIGH);
}

void motorDisable() {
    analogWrite(PIN_RPWM, 0);
    analogWrite(PIN_LPWM, 0);

    digitalWrite(PIN_REN, LOW);
    digitalWrite(PIN_LEN, LOW);
}

void motorOpen(int pwm) {
    pwm = clampPwm(pwm);

    motorEnable();

    analogWrite(PIN_LPWM, 0);
    analogWrite(PIN_RPWM, pwm);
}

void motorClose(int pwm) {
    pwm = clampPwm(pwm);

    motorEnable();

    analogWrite(PIN_RPWM, 0);
    analogWrite(PIN_LPWM, pwm);
}

void motorStop() {
    analogWrite(PIN_RPWM, 0);
    analogWrite(PIN_LPWM, 0);
}
