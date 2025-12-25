#pragma once
#include <stdint.h>

// Init ADC hardware
void adcInit();

// Read ADC by Arduino pin (A0, A1, A2...)
uint16_t readADC(int pin);
