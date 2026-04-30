#include "comm/protokol.h"

#include "comm/params.h"
#include "control.h"
#include "canbus.h"
#include "config.h"

#include <Arduino.h>
#include <string.h>

static char line[96];
static uint8_t idx = 0;

static void handleCommand(char* cmd) {
    // SET Kp 4.5
    if (strncmp(cmd, "SET ", 4) == 0) {
        char* key = strtok(cmd + 4, " ");
        char* val = strtok(NULL, " ");

        if (!key || !val) {
            SerialUSB.println("ERR");
            return;
        }

        setParam(key, val);
        if (strcmp(key, "LINK_ECU_CAN_BITRATE") == 0) {
            canbusInit((uint32_t)LINK_ECU_CAN_BITRATE);
        }

        SerialUSB.println("OK");
        return;
    }

    // GET ALL
    if (strcmp(cmd, "GET ALL") == 0) {
        dumpParams();
        return;
    }

    // CLEAR FAULT
    if (strcmp(cmd, "CLEAR FAULT") == 0) {
        SerialUSB.println(controlClearFault() ? "OK" : "ERR");
        return;
    }

    // GET FAULT
    if (strcmp(cmd, "GET FAULT") == 0) {
        SerialUSB.print("FAULT ");
        SerialUSB.print(controlFaultLatched() ? 1 : 0);
        SerialUSB.print(" REASON=");
        SerialUSB.println(controlFaultReasonText());
        return;
    }

    // GET ECU
    if (strcmp(cmd, "GET ECU") == 0) {
        LinkEcuData data;
        canbusGetLinkEcuData(data);

        SerialUSB.print("ECU RPM=");
        if (data.rpmValid) {
            SerialUSB.print(data.rpm);
        } else {
            SerialUSB.print("NA");
        }

        SerialUSB.print(" ECT_C=");
        if (data.coolantTempValid) {
            SerialUSB.print(data.coolantTempC, 1);
        } else {
            SerialUSB.print("NA");
        }

        SerialUSB.println();
        return;
    }

    SerialUSB.println("ERR");
}

void protocolFeed(char c) {
    if (c == '\r') {
        return;
    }

    if (c == '\n') {
        line[idx] = 0;
        idx = 0;

        if (line[0] != 0) {
            handleCommand(line);
        }

        return;
    }

    if (idx < sizeof(line) - 1) {
        line[idx++] = c;
    }
}
