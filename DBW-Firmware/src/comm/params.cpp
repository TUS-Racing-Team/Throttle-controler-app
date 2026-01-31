#include "comm/params.h"
#include <Arduino.h>
#include <stdlib.h>
#include <string.h>

// ===== ТЕЗИ ПАРАМЕТРИ СЕ ПОЛЗВАТ ОТ CONTROL =====
float IDLE_POS = 7.5f;
float FAR_ZONE   = 10.0f;
float DEADBAND   = 1.0f;
int   PWM_FAR    = 110;
int   PWM_MIN    = 40;
int   PWM_NEAR_MAX = 90;

// PID parameters
float Kp = 2.0f;    // Proportional gain
float Ki = 0.1f;    // Integral gain
float Kd = 0.5f;    // Derivative gain

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
}
