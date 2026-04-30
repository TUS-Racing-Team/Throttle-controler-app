#include <Arduino.h>

#include "adc.h"
#include "motor.h"
#include "control.h"
#include "apps.h"
#include "tps.h"
#include "comm/comm.h"

void setup() {
    Serial.begin(115200);
    SerialUSB.begin(115200);

    analogReadResolution(12);
    analogWriteResolution(8);

    adcInit();
    motorInit();
    controlInit();
    commInit();

    Serial.println("DBW firmware started");
}

void loop() {
    // controlTP() self-schedules at 1 kHz.
    controlTP();

    // Keep comm non-blocking.
    if (commIsActive()) {
        commUpdate();
    }

    // No delay here: delay() adds jitter to the control loop.
}
