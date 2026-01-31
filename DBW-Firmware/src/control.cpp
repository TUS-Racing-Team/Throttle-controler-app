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

// PID state variables
static float prevError = 0;
static float integralError = 0;
static unsigned long lastUpdateTime = 0;

// Timers for sensor error handling
static unsigned long tpsPosErrorStart = 0;
static unsigned long appsPosErrorStart = 0;

// control parameters are declared in include/config.h and defined in params.cpp

// Use ReadData from include/read_data.h

void controlInit() {
    posF = 0;
    tpsPosErrorStart = 0;
    prevError = 0;
    integralError = 0;
    lastUpdateTime = millis();
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
    Serial.print("Target: "); Serial.print(target); Serial.print(" TPS Pos: "); Serial.print(pos); Serial.print(" Valid: "); Serial.println(valid);
    
    // Handle sensor errors: if `valid` is for more than 500ms, stop the motor
    if (!valid) {
        if (tpsPosErrorStart == 0) {
            tpsPosErrorStart = millis();
            // keep updating but this position can't be trusted 
            posF += alpha * (pos - posF);    
        } else if ((unsigned long)(millis() - tpsPosErrorStart) > 500UL) {
            // persistent error — stop motor
            motorStop();
            integralError = 0;  // Reset integral on error
            prevError = 0;
            return;
        }
        // Don't update filtered position while we have an intermittent sensor error
    } else {
        // valid reading — reset error timer and update filtered position
        tpsPosErrorStart = 0;
        posF += alpha * (pos - posF);
    }

    // Calculate time delta for PID
    unsigned long currentTime = millis();
    float deltaTime = (currentTime - lastUpdateTime) / 1000.0f;  // Convert to seconds
    if (deltaTime < 0.001f) deltaTime = 0.001f;  // Minimum 1ms
    lastUpdateTime = currentTime;

    float err = target - posF;
    float eAbs = fabsf(err);

    // If error is within deadband, gradually reduce integral and stop
    if (eAbs <= DEADBAND) {
        integralError *= 0.9f;  // Decay integral to prevent windup
        if (integralError < 0.01f) integralError = 0;
        prevError = err;
        motorStop();
        return;
    }

    // PID calculation
    float P = Kp * err;                          // Proportional term
    integralError += err * deltaTime;            // Accumulate error over time
    integralError = constrain(integralError, -50.0f, 50.0f);  // Anti-windup
    float I = Ki * integralError;                // Integral term
    float D = Kd * (err - prevError) / deltaTime;  // Derivative term (dampening)
    
    float pidOutput = P + I + D;
    
    // Convert PID output to PWM (clamp between PWM_MIN and PWM_FAR)
    int pwm = (int)constrain(PWM_MIN + pidOutput, PWM_MIN, PWM_FAR);
    
    Serial.print("P: "); Serial.print(P);
    Serial.print(" I: "); Serial.print(I);
    Serial.print(" D: "); Serial.print(D);
    Serial.print(" PID: "); Serial.print(pidOutput);
    Serial.print(" PWM: "); Serial.println(pwm);
    
    prevError = err;

    if (err > 0) motorOpen(pwm);
    else motorClose(pwm);
}
