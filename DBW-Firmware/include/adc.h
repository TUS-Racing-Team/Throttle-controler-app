#pragma once

#include <Arduino.h>

// Optional direct ADC functions.
// Firmware currently uses Arduino analogRead() with analogReadResolution(12).
void adcInit();
uint16_t readADC(int pin);
