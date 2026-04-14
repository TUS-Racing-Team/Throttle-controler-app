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

// Filtered target (setpoint)
static float targetF = 0;
static float prevPosF = 0;
static int printCounter = 0;

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
    targetF = IDLE_POS;
    prevPosF = 0;
    printCounter = 0;
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
    } 

    return ReadData{appsData.pos, true};
}

void controlTP() {
    ReadData appsData = readApps();
    float wanted = appsData.pos;
    float target = (wanted < (int)ceil(IDLE_POS)) ? IDLE_POS : wanted;

    ReadData tpsData = readThrottlePct();
    float pos = tpsData.pos;
    bool valid = tpsData.valid;
    // Serial.print("Target: "); Serial.print(target); Serial.print(" TPS Pos: "); Serial.print(pos); Serial.print(" Valid: "); Serial.println(valid);
    
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

    // Smooth and limit setpoint changes to avoid jumps when moving APPS
    float targetNext = targetF + CMD_ALPHA * (target - targetF);
    float maxDelta = CMD_SLEW_RATE * deltaTime; // percent per second -> percent per this step
    float diff = targetNext - targetF;
    if (diff > maxDelta) diff = maxDelta;
    else if (diff < -maxDelta) diff = -maxDelta;
    targetF += diff;

    float err = targetF - posF;

    // Print only frequency (Hz) and difference between APPS and filtered target, but not every loop
    float hz = 1.0f / deltaTime;
    printCounter++;
    if (printCounter >= 5) {
        Serial.print("Hz "); Serial.print(hz); Serial.print(" Diff "); Serial.println(wantedPos - targetF);
        printCounter = 0;
    }
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

    // Derivative: use measurement derivative (posF) to avoid large spikes when setpoint changes
    float measDeriv = (posF - prevPosF) / deltaTime; // percent/sec
    float D = -Kd * measDeriv;
    D = constrain(D, -30.0f, 30.0f); // clamp D to avoid huge kicks

    float pidOutput = P + I + D;
    
    // Convert PID output to PWM (clamp between PWM_MIN and PWM_FAR)
    int pwm = (int)constrain(PWM_MIN + pidOutput, PWM_MIN, PWM_FAR);
    
    // Serial.print("P: "); Serial.print(P);
    // Serial.print(" I: "); Serial.print(I);
    // Serial.print(" D: "); Serial.print(D);
    // Serial.print(" PID: "); Serial.print(pidOutput);
    // Serial.print(" PWM: "); Serial.println(pwm);
    
    prevError = err;
    prevPosF = posF;

    if (err > 0) motorOpen(pwm);
    else motorClose(pwm);
}
