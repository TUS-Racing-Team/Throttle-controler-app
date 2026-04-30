#include "comm/params.h"

#include "config.h"

#include <Arduino.h>
#include <stdlib.h>
#include <string.h>

// =====================
// Control defaults
// =====================

float IDLE_POS = 7.5f;
float FAR_ZONE = 10.0f;
float DEADBAND = 1.0f;

// PWM settings for BTS7960.
// Start conservative. Increase PWM_FAR only after bench testing.
int PWM_MIN = 48;
int PWM_NEAR_MAX = 100;
int PWM_FAR = 220;

// Measured TPS low-pass filter.
float alpha = 0.25f;

// PID starting values.
// Tune on bench: start with Ki=0, Kd=0, tune Kp, then add Kd, then small Ki.
float Kp = 5.0f;
float Ki = 0.8f;
float Kd = 0.0f;

// Command filter/slew.
float CMD_ALPHA = 0.75f;
float CMD_SLEW_RATE = 700.0f;

// Fault timing.
float APPS_FAULT_TIME_MS = 100.0f;
float TPS_FAULT_TIME_MS = 100.0f;
float TRACKING_FAULT_TIME_MS = 500.0f;
float TRACKING_ERROR_LIMIT = 10.0f;

// PID safety limits.
float INTEGRAL_LIMIT = 80.0f;
float D_FILTER_ALPHA = 0.15f;
float OUTPUT_LIMIT = 255.0f;

// ECU analog output on Arduino Due DAC0.
float ECU_DAC_IDLE_PCT = 7.5f;
float ECU_DAC_MIN_RAW = 0.0f;
float ECU_DAC_MAX_RAW = 4095.0f;

// Link ECU Generic Dash defaults. Match these with PCLink CAN Setup.
int LINK_ECU_CAN_BITRATE = 1000000;
int LINK_ECU_CAN_ID = 56;
int LINK_ECU_CAN_TIMEOUT_MS = 250;

static bool setFloat(const char* key, const char* expected, const char* value, float& target) {
    if (strcmp(key, expected) == 0) {
        target = atof(value);
        return true;
    }

    return false;
}

static bool setInt(const char* key, const char* expected, const char* value, int& target) {
    if (strcmp(key, expected) == 0) {
        target = (int)strtol(value, NULL, 0);
        return true;
    }

    return false;
}

void setParam(const char* key, const char* value) {
    if (setFloat(key, "IDLE_POS", value, IDLE_POS)) return;
    if (setFloat(key, "FAR_ZONE", value, FAR_ZONE)) return;
    if (setFloat(key, "DEADBAND", value, DEADBAND)) return;

    if (setInt(key, "PWM_MIN", value, PWM_MIN)) return;
    if (setInt(key, "PWM_NEAR_MAX", value, PWM_NEAR_MAX)) return;
    if (setInt(key, "PWM_FAR", value, PWM_FAR)) return;

    if (setFloat(key, "alpha", value, alpha)) return;

    if (setFloat(key, "Kp", value, Kp)) return;
    if (setFloat(key, "Ki", value, Ki)) return;
    if (setFloat(key, "Kd", value, Kd)) return;

    if (setFloat(key, "CMD_ALPHA", value, CMD_ALPHA)) return;
    if (setFloat(key, "CMD_SLEW_RATE", value, CMD_SLEW_RATE)) return;

    if (setFloat(key, "APPS_FAULT_TIME_MS", value, APPS_FAULT_TIME_MS)) return;
    if (setFloat(key, "TPS_FAULT_TIME_MS", value, TPS_FAULT_TIME_MS)) return;
    if (setFloat(key, "TRACKING_FAULT_TIME_MS", value, TRACKING_FAULT_TIME_MS)) return;
    if (setFloat(key, "TRACKING_ERROR_LIMIT", value, TRACKING_ERROR_LIMIT)) return;

    if (setFloat(key, "INTEGRAL_LIMIT", value, INTEGRAL_LIMIT)) return;
    if (setFloat(key, "D_FILTER_ALPHA", value, D_FILTER_ALPHA)) return;
    if (setFloat(key, "OUTPUT_LIMIT", value, OUTPUT_LIMIT)) return;

    if (setFloat(key, "ECU_DAC_IDLE_PCT", value, ECU_DAC_IDLE_PCT)) return;
    if (setFloat(key, "ECU_DAC_MIN_RAW", value, ECU_DAC_MIN_RAW)) return;
    if (setFloat(key, "ECU_DAC_MAX_RAW", value, ECU_DAC_MAX_RAW)) return;

    if (setInt(key, "LINK_ECU_CAN_BITRATE", value, LINK_ECU_CAN_BITRATE)) return;
    if (setInt(key, "LINK_ECU_CAN_ID", value, LINK_ECU_CAN_ID)) return;
    if (setInt(key, "LINK_ECU_CAN_TIMEOUT_MS", value, LINK_ECU_CAN_TIMEOUT_MS)) return;

    if (setInt(key, "app1Min", value, app1Min)) return;
    if (setInt(key, "app1Max", value, app1Max)) return;
    if (setInt(key, "app2Min", value, app2Min)) return;
    if (setInt(key, "app2Max", value, app2Max)) return;

    if (setInt(key, "tps1Min", value, tps1Min)) return;
    if (setInt(key, "tps1Max", value, tps1Max)) return;
    if (setInt(key, "tps2Min", value, tps2Min)) return;
    if (setInt(key, "tps2Max", value, tps2Max)) return;
}

static void printParam(const char* name, float value) {
    SerialUSB.print(name);
    SerialUSB.print(" ");
    SerialUSB.println(value);
}

static void printParam(const char* name, int value) {
    SerialUSB.print(name);
    SerialUSB.print(" ");
    SerialUSB.println(value);
}

void dumpParams() {
    printParam("IDLE_POS", IDLE_POS);
    printParam("FAR_ZONE", FAR_ZONE);
    printParam("DEADBAND", DEADBAND);

    printParam("PWM_MIN", PWM_MIN);
    printParam("PWM_NEAR_MAX", PWM_NEAR_MAX);
    printParam("PWM_FAR", PWM_FAR);

    printParam("alpha", alpha);

    printParam("Kp", Kp);
    printParam("Ki", Ki);
    printParam("Kd", Kd);

    printParam("CMD_ALPHA", CMD_ALPHA);
    printParam("CMD_SLEW_RATE", CMD_SLEW_RATE);

    printParam("APPS_FAULT_TIME_MS", APPS_FAULT_TIME_MS);
    printParam("TPS_FAULT_TIME_MS", TPS_FAULT_TIME_MS);
    printParam("TRACKING_FAULT_TIME_MS", TRACKING_FAULT_TIME_MS);
    printParam("TRACKING_ERROR_LIMIT", TRACKING_ERROR_LIMIT);

    printParam("INTEGRAL_LIMIT", INTEGRAL_LIMIT);
    printParam("D_FILTER_ALPHA", D_FILTER_ALPHA);
    printParam("OUTPUT_LIMIT", OUTPUT_LIMIT);

    printParam("ECU_DAC_IDLE_PCT", ECU_DAC_IDLE_PCT);
    printParam("ECU_DAC_MIN_RAW", ECU_DAC_MIN_RAW);
    printParam("ECU_DAC_MAX_RAW", ECU_DAC_MAX_RAW);

    printParam("LINK_ECU_CAN_BITRATE", LINK_ECU_CAN_BITRATE);
    printParam("LINK_ECU_CAN_ID", LINK_ECU_CAN_ID);
    printParam("LINK_ECU_CAN_TIMEOUT_MS", LINK_ECU_CAN_TIMEOUT_MS);

    printParam("app1Min", app1Min);
    printParam("app1Max", app1Max);
    printParam("app2Min", app2Min);
    printParam("app2Max", app2Max);

    printParam("tps1Min", tps1Min);
    printParam("tps1Max", tps1Max);
    printParam("tps2Min", tps2Min);
    printParam("tps2Max", tps2Max);
}
