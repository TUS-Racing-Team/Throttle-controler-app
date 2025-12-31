#include "control.h"
#include "motor.h"
#include "tps.h"
#include "apps.h"
#include "config.h"
#include "read_data.h"
#include <Arduino.h>
#include <math.h>

float alpha = 0.20f;
float posF = 0;

// Timers for sensor error handling
static unsigned long tpsPosErrorStart = 0;
static unsigned long appsPosErrorStart = 0;

const float IDLE_POS = 7.5f;
const float FAR_ZONE = 10.0f;
const float DEADBAND = 1.0f;

const int PWM_MIN = 40;
const int PWM_NEAR_MAX = 90;
const int PWM_FAR = 110;

// Use ReadData from include/read_data.h

void controlInit() {
    posF = 0;
    tpsPosErrorStart = 0;
}

ReadData readApps() {
    ReadData appsData = readAppsPct();
    
    if (!appsData.valid) {
        if (appsPosErrorStart == 0) {
            appsPosErrorStart = millis();
            // keep updating but this possition can't be trusted 
            return ReadData{appsData.pos, false};
        } else if ((unsigned long)(millis() - appsPosErrorStart) > 500UL) {
            // persistent error — stop motor
            motorStop();
            return ReadData{appsData.pos, false};
        }
        // Don't update filtered position while we have an intermittent sensor error
        return ReadData{appsData.pos, false};
    } else {
        // valid reading — reset error timer and update filtered position
        return ReadData{appsData.pos, true};
    }
}

void controlUpdate(int wantedPos) {
    float target = (wantedPos < (int)ceil(IDLE_POS)) ? IDLE_POS : (float)wantedPos;

    ReadData tpsData = readThrottlePct();
    float pos = tpsData.pos;
    bool valid = tpsData.valid;

    // Handle sensor errors: if `valid` is for more than 500ms, stop the motor
    if (!valid) {
        if (tpsPosErrorStart == 0) {
            tpsPosErrorStart = millis();
            // keep updating but this possition can't be trusted 
            posF += alpha * (pos - posF);    
        } else if ((unsigned long)(millis() - tpsPosErrorStart) > 500UL) {
            // persistent error — stop motor
            motorStop();
            return;
        }
        // Don't update filtered position while we have an intermittent sensor error
    } else {
        // valid reading — reset error timer and update filtered position
        tpsPosErrorStart = 0;
        posF += alpha * (pos - posF);
    }

    float err = target - posF;
    float eAbs = fabsf(err);

    if (eAbs <= DEADBAND) {
        // motorStop();
        return;
    }

    int pwm;
    if (eAbs > FAR_ZONE) {
        pwm = PWM_FAR;
    } else {
        float t = eAbs / FAR_ZONE;
        pwm = (int)(PWM_MIN + t * (PWM_NEAR_MAX - PWM_MIN));
    }

    if (err > 0) motorOpen(pwm);
    else motorClose(pwm);
}
