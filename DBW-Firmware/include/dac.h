#pragma once
#include <stdint.h>

// Инициализация на DAC
void dacInit();

// Задаване на throttle към ECU (в %)
void dacSetThrottlePct(float pct);

// (по избор) директно DAC value
void dacSetRaw(uint16_t value);
