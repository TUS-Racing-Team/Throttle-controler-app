#include "comm/comm.h"
#include "comm/protokol.h"
#include <Arduino.h>

void commInit() {
    SerialUSB.begin(115200);
}

bool commIsActive() {
    return SerialUSB;   // Native USB CDC
}

void commUpdate() {
    if (!SerialUSB) return;

    while (SerialUSB.available()) {
        char c = SerialUSB.read();
        protocolFeed(c);
    }
}
