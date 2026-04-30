#include "comm/comm.h"

#include "comm/protokol.h"

#include <Arduino.h>

static constexpr uint8_t MAX_BYTES_PER_UPDATE = 32;

void commInit() {
    SerialUSB.begin(115200);
}

bool commIsActive() {
    return SerialUSB;
}

void commUpdate() {
    if (!SerialUSB) {
        return;
    }

    uint8_t bytesLeft = MAX_BYTES_PER_UPDATE;
    while (bytesLeft-- > 0 && SerialUSB.available()) {
        char c = SerialUSB.read();
        protocolFeed(c);
    }
}
