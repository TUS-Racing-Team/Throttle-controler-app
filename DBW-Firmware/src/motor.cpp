#include "motor.h"
#include "pins.h"
#include <Arduino.h>

void motorInit() {
    pinMode(PIN_RPWM, OUTPUT);
    pinMode(PIN_LPWM, OUTPUT);
    pinMode(PIN_REN, OUTPUT);
    pinMode(PIN_LEN, OUTPUT);

    digitalWrite(PIN_REN, HIGH);
    digitalWrite(PIN_LEN, HIGH);
}

void motorOpen(int pwm) {
    analogWrite(PIN_LPWM, 0);
    analogWrite(PIN_RPWM, pwm);
}

void motorClose(int pwm) {
    analogWrite(PIN_RPWM, 0);
    analogWrite(PIN_LPWM, pwm);
}

void motorStop() {
    analogWrite(PIN_RPWM, 0);
    analogWrite(PIN_LPWM, 0);
}
