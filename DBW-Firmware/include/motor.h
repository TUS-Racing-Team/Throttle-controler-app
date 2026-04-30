#pragma once

void motorInit();

void motorEnable();
void motorDisable();

void motorOpen(int pwm);
void motorClose(int pwm);

// PWM = 0, driver still enabled.
void motorStop();
