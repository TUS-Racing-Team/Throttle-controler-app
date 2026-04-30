#include "canbus.h"

#include "config.h"

#include <Arduino.h>
#include <due_can.h>

static CanFrame queue[64];
static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;
static volatile uint32_t droppedFrames = 0;

static LinkEcuData linkEcuData{};

static uint16_t readU16Le(const uint8_t* data) {
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

static inline bool push(const CanFrame& f) {
    uint8_t n = (head + 1) & 63;

    if (n == tail) {
        droppedFrames++;
        return false;
    }

    queue[head] = f;
    head = n;

    return true;
}

static void decodeLinkGenericDash(const CanFrame& f) {
    if (f.extended || f.id != (uint32_t)LINK_ECU_CAN_ID || f.len < 8) {
        return;
    }

    // Link Generic Dash is a multiplexed stream. Byte 0 is the frame index,
    // byte 1 is 0, and values are 16-bit little-endian.
    if (f.data[1] != 0) {
        return;
    }

    const uint32_t nowMs = millis();

    switch (f.data[0]) {
        case 0: {
            linkEcuData.rpm = readU16Le(&f.data[2]);
            linkEcuData.rpmValid = true;
            linkEcuData.lastRpmMs = nowMs;
            break;
        }

        case 2: {
            const uint16_t rawCoolantTemp = readU16Le(&f.data[6]);
            linkEcuData.coolantTempC = (float)rawCoolantTemp - 50.0f;
            linkEcuData.coolantTempValid = true;
            linkEcuData.lastCoolantTempMs = nowMs;
            break;
        }

        default:
            break;
    }
}

bool canbusPop(CanFrame& out) {
    if (tail == head) {
        return false;
    }

    out = queue[tail];
    tail = (tail + 1) & 63;

    return true;
}

bool canbusGetLinkEcuData(LinkEcuData& out) {
    out = linkEcuData;

    const uint32_t nowMs = millis();
    const uint32_t timeoutMs = (uint32_t)LINK_ECU_CAN_TIMEOUT_MS;

    if (!out.rpmValid || (uint32_t)(nowMs - out.lastRpmMs) > timeoutMs) {
        out.rpmValid = false;
    }

    if (!out.coolantTempValid || (uint32_t)(nowMs - out.lastCoolantTempMs) > timeoutMs) {
        out.coolantTempValid = false;
    }

    return out.rpmValid || out.coolantTempValid;
}

uint32_t canbusDroppedFrames() {
    return droppedFrames;
}

void canbusInit(uint32_t bitrate) {
    head = 0;
    tail = 0;
    droppedFrames = 0;
    linkEcuData = LinkEcuData{};

    Can0.begin(bitrate);
    Can0.watchFor();
    Can0.setNumTXBoxes(1);
}

void canbusUpdate() {
    CAN_FRAME f;

    int maxFrames = 8;

    while (maxFrames-- > 0 && Can0.available()) {
        Can0.read(f);

        CanFrame out{};
        out.id = f.id;
        out.extended = f.extended;
        out.len = f.length;

        if (out.len > 8) {
            out.len = 8;
        }

        for (int i = 0; i < out.len; i++) {
            out.data[i] = f.data.bytes[i];
        }

        decodeLinkGenericDash(out);
        push(out);
    }
}
