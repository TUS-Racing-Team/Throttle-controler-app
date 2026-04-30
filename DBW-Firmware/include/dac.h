#pragma once

#include <Arduino.h>

void dacInit();
void dacSetThrottlePct(float pct);
void dacSetRaw(uint16_t value);
