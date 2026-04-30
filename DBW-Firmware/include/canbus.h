#pragma once

#include <Arduino.h>
#include <due_can.h>

struct CanFrame {
    uint32_t id;
    bool extended;
    uint8_t len;
    uint8_t data[8];
};

void canbusInit(uint32_t bitrate);
void canbusUpdate();
bool canbusPop(CanFrame& out);
