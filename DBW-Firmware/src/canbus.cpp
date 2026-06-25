#include "canbus.h"

#include "config.h"

#include <Arduino.h>
#include <due_can.h>

static CanFrame queue[64];
static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;
static volatile uint32_t droppedFrames = 0;

static LinkEcuData linkEcuData{};
static uint32_t lastCanStatusPrintMs = 0;
static uint32_t lastCanSummaryPrintMs = 0;
static uint32_t linkFrameCount = 0;
static uint32_t lastSummaryFrameCount = 0;
static uint32_t lastLinkFrameMs = 0;
static CanFrame lastLinkFrame{};

static constexpr uint32_t CAN_SUMMARY_PRINT_INTERVAL_MS = 200;

static uint16_t readU16Le(const uint8_t* data) {
    return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

static bool isLinkFrame(const CanFrame& f) {
    return !f.extended && f.id == (uint32_t)LINK_ECU_CAN_ID && f.len >= 8;
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
    if (!isLinkFrame(f)) {
        return;
    }

    // Link Generic Dash is a multiplexed stream. Byte 0 is the frame index,
    // byte 1 is 0, and values are 16-bit little-endian.
    if (f.data[1] != 0) {
        return;
    }

    const uint32_t nowMs = millis();
    lastLinkFrame = f;
    lastLinkFrameMs = nowMs;
    linkFrameCount++;

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

static void printLinkValue(Print& stream, bool valid, uint16_t value) {
    if (valid) {
        stream.print(value);
    } else {
        stream.print("NA");
    }
}

static void printLinkValue(Print& stream, bool valid, float value, uint8_t decimals) {
    if (valid) {
        stream.print(value, decimals);
    } else {
        stream.print("NA");
    }
}

static void printLinkSummary(Print& stream, const LinkEcuData& data, uint32_t framesPerSecond) {
    stream.print("ECU rpm=");
    printLinkValue(stream, data.rpmValid, data.rpm);
    stream.print(" ectC=");
    printLinkValue(stream, data.coolantTempValid, data.coolantTempC, 1);
    stream.print(" rxRate=");
    stream.print(framesPerSecond);
    stream.print("Hz");

    if (lastLinkFrameMs != 0) {
        stream.print(" mux=");
        stream.print(lastLinkFrame.data[0]);
        stream.print(" raw16=");
        stream.print(readU16Le(&lastLinkFrame.data[2]));
        stream.print(",");
        stream.print(readU16Le(&lastLinkFrame.data[4]));
        stream.print(",");
        stream.print(readU16Le(&lastLinkFrame.data[6]));
    } else {
        stream.print(" mux=NA raw16=NA");
    }

    stream.print(" dropped=");
    stream.println(droppedFrames);
}

static void printLinkSummaryIfDue() {
    const uint32_t nowMs = millis();

    if ((uint32_t)(nowMs - lastCanSummaryPrintMs) < CAN_SUMMARY_PRINT_INTERVAL_MS) {
        return;
    }

    const uint32_t elapsedMs = (lastCanSummaryPrintMs == 0)
        ? CAN_SUMMARY_PRINT_INTERVAL_MS
        : (uint32_t)(nowMs - lastCanSummaryPrintMs);
    const uint32_t frameDelta = linkFrameCount - lastSummaryFrameCount;
    const uint32_t framesPerSecond = (frameDelta * 1000UL) / elapsedMs;

    lastCanSummaryPrintMs = nowMs;
    lastSummaryFrameCount = linkFrameCount;

    LinkEcuData data;
    canbusGetLinkEcuData(data);

    printLinkSummary(Serial, data, framesPerSecond);
    printLinkSummary(SerialUSB, data, framesPerSecond);
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
    lastCanStatusPrintMs = 0;
    lastCanSummaryPrintMs = 0;
    linkFrameCount = 0;
    lastSummaryFrameCount = 0;
    lastLinkFrameMs = 0;
    lastLinkFrame = CanFrame{};
    linkEcuData = LinkEcuData{};

    const uint32_t startedBitrate = Can0.begin(bitrate);
    Can0.setNumTXBoxes(1);
    Can0.setRXFilter(0, 0, 0, false);
    Can0.setRXFilter(1, 0, 0, true);

    Serial.print("CAN init bitrate=");
    Serial.print(bitrate);
    Serial.print(" started=");
    Serial.print(startedBitrate);
    Serial.print(" targetId=");
    Serial.println(LINK_ECU_CAN_ID);

    SerialUSB.print("CAN init bitrate=");
    SerialUSB.print(bitrate);
    SerialUSB.print(" started=");
    SerialUSB.print(startedBitrate);
    SerialUSB.print(" targetId=");
    SerialUSB.println(LINK_ECU_CAN_ID);
}

void canbusUpdate() {
    CAN_FRAME f;

    int maxFrames = 8;

    const uint32_t nowMs = millis();
    if (linkFrameCount == 0 && (uint32_t)(nowMs - lastCanStatusPrintMs) >= 1000) {
        lastCanStatusPrintMs = nowMs;

        Serial.print("CAN status avail=");
        Serial.print(Can0.available());
        Serial.print(" sr=0x");
        Serial.print(Can0.get_status(), HEX);
        Serial.print(" imr=0x");
        Serial.print(Can0.get_interrupt_mask(), HEX);
        Serial.print(" rxErr=");
        Serial.print(Can0.get_rx_error_cnt());
        Serial.print(" txErr=");
        Serial.println(Can0.get_tx_error_cnt());

        SerialUSB.print("CAN status avail=");
        SerialUSB.print(Can0.available());
        SerialUSB.print(" sr=0x");
        SerialUSB.print(Can0.get_status(), HEX);
        SerialUSB.print(" imr=0x");
        SerialUSB.print(Can0.get_interrupt_mask(), HEX);
        SerialUSB.print(" rxErr=");
        SerialUSB.print(Can0.get_rx_error_cnt());
        SerialUSB.print(" txErr=");
        SerialUSB.println(Can0.get_tx_error_cnt());
    }

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

    printLinkSummaryIfDue();
}
