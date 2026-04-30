#pragma once

#include <Arduino.h>
#include <due_can.h>

struct CanFrame {
    uint32_t id;
    bool extended;
    uint8_t len;
    uint8_t data[8];
};

struct LinkEcuData {
    uint16_t rpm;
    float coolantTempC;
    bool rpmValid;
    bool coolantTempValid;
    uint32_t lastRpmMs;
    uint32_t lastCoolantTempMs;
};

void canbusInit(uint32_t bitrate);
void canbusUpdate();
bool canbusPop(CanFrame& out);
bool canbusGetLinkEcuData(LinkEcuData& out);
uint32_t canbusDroppedFrames();
