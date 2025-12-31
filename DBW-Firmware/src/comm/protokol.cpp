#include "comm/protokol.h"
#include "comm/params.h"
#include <Arduino.h>
#include <stdlib.h>
#include <string.h>

static char line[64];
static uint8_t idx = 0;

static void handleCommand(char* cmd) {

    // SET PWM_FAR 110
    if (strncmp(cmd, "SET ", 4) == 0) {
        char* key = strtok(cmd + 4, " ");
        char* val = strtok(NULL, " ");

        if (!key || !val) return;

        setParam(key, val);
        SerialUSB.println("OK");
        return;
    }

    // GET ALL
    if (strcmp(cmd, "GET ALL") == 0) {
        dumpParams();
        return;
    }

    SerialUSB.println("ERR");
}

void protocolFeed(char c) {
    if (c == '\r') return;

    if (c == '\n') {
        line[idx] = 0;
        idx = 0;
        handleCommand(line);
        return;
    }

    if (idx < sizeof(line) - 1) {
        line[idx++] = c;
    }
}
