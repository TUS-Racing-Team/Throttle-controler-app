#include "adc.h"

void adcInit() {
    // Arduino Due ADC resolution is configured in main.cpp with analogReadResolution(12).
    // Keep this function for compatibility with the existing project structure.
}

uint16_t readADC(int pin) {
    return (uint16_t)analogRead(pin);
}
