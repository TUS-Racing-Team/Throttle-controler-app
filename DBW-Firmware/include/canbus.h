#pragma once
#include <stdint.h>
#include <stdbool.h>

struct CanFrame {
    uint32_t id;
    bool extended;
    uint8_t len;
    uint8_t data[8];
};

void canbusInit(uint32_t bitrate);
void canbusUpdate();                 // non-blocking
bool canbusPop(CanFrame& out);       // взимаш кадър
