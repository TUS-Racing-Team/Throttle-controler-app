#include "comm/params.h"
#include <Arduino.h>
#include <stdlib.h>
#include <string.h>

// ===== ТЕЗИ ПАРАМЕТРИ СЕ ПОЛЗВАТ ОТ CONTROL =====
float IDLE_POS = 7.5f;
float FAR_ZONE   = 10.0f;
float DEADBAND   = 1.5f;      // larger zone to prevent oscillation
int   PWM_FAR    = 110;
int   PWM_MIN    = 40;
int   PWM_NEAR_MAX = 90;

// PID parameters
float Kp = 1.0f;    // Proportional gain
float Ki = 0.3f;    // Integral gain
float Kd = 1.0f;    // Derivative gain (reduced)

// Command filter defaults
float CMD_ALPHA = 0.25f;      // smoothing factor for incoming command (higher = faster tracking)
float CMD_SLEW_RATE = 800.0f; // percent per second maximum change (faster response)

void setParam(const char* key, const char* value) {

    if (strcmp(key, "FAR_ZONE") == 0) {
        FAR_ZONE = atof(value);
        return;
    }

    if (strcmp(key, "DEADBAND") == 0) {
        DEADBAND = atof(value);
        return;
    }

    if (strcmp(key, "PWM_FAR") == 0) {
        PWM_FAR = atoi(value);
        return;
    }

    if (strcmp(key, "PWM_MIN") == 0) {
        PWM_MIN = atoi(value);
        return;
    }

    if (strcmp(key, "PWM_NEAR_MAX") == 0) {
        PWM_NEAR_MAX = atoi(value);
        return;
    }

    if (strcmp(key, "Kp") == 0) {
        Kp = atof(value);
        return;
    }

    if (strcmp(key, "Ki") == 0) {
        Ki = atof(value);
        return;
    }

    if (strcmp(key, "Kd") == 0) {
        Kd = atof(value);
        return;
    }
    if (strcmp(key, "CMD_ALPHA") == 0) {
        CMD_ALPHA = atof(value);
        return;
    }

    if (strcmp(key, "CMD_SLEW_RATE") == 0) {
        CMD_SLEW_RATE = atof(value);
        return;
    }
}

void dumpParams() {
    SerialUSB.print("FAR_ZONE "); SerialUSB.println(FAR_ZONE);
    SerialUSB.print("DEADBAND "); SerialUSB.println(DEADBAND);
    SerialUSB.print("PWM_FAR "); SerialUSB.println(PWM_FAR);
    SerialUSB.print("PWM_MIN "); SerialUSB.println(PWM_MIN);
    SerialUSB.print("PWM_NEAR_MAX "); SerialUSB.println(PWM_NEAR_MAX);
    SerialUSB.print("Kp "); SerialUSB.println(Kp);
    SerialUSB.print("Ki "); SerialUSB.println(Ki);
    SerialUSB.print("Kd "); SerialUSB.println(Kd);
    SerialUSB.print("CMD_ALPHA "); SerialUSB.println(CMD_ALPHA);
    SerialUSB.print("CMD_SLEW_RATE "); SerialUSB.println(CMD_SLEW_RATE);
}
