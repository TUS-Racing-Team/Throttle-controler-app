#include "control.h"

#include "motor.h"
#include "dac.h"
#include "tps.h"
#include "apps.h"
#include "config.h"
#include "read_data.h"

#include <Arduino.h>
#include <math.h>

static constexpr float LOOP_DT = 0.001f;
static constexpr uint32_t LOOP_PERIOD_US = 1000;

static float posF = 0.0f;
static float targetF = 0.0f;

static float integralError = 0.0f;
static float prevPosF = 0.0f;
static float dFilt = 0.0f;

static uint32_t lastLoopUs = 0;
static uint32_t appsFaultStartMs = 0;
static uint32_t tpsFaultStartMs = 0;
static uint32_t trackingFaultStartMs = 0;

static bool faultLatched = false;

static uint16_t debugCounter = 0;

static float clampf(float x, float lo, float hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

static float slewLimit(float current, float target, float ratePctPerSec, float dt) {
    const float maxStep = ratePctPerSec * dt;
    float diff = target - current;

    if (diff > maxStep) diff = maxStep;
    if (diff < -maxStep) diff = -maxStep;

    return current + diff;
}

static void resetPid() {
    integralError = 0.0f;
    dFilt = 0.0f;
    prevPosF = posF;
}

static void latchFault() {
    faultLatched = true;
    resetPid();
    dacSetThrottlePct(ECU_DAC_IDLE_PCT);

    // Real fault behavior: disable driver enable pins.
    motorDisable();
}

static bool persistentFault(bool condition, uint32_t& startMs, float faultTimeMs) {
    const uint32_t nowMs = millis();

    if (!condition) {
        startMs = 0;
        return false;
    }

    if (startMs == 0) {
        startMs = nowMs;
        return false;
    }

    return (uint32_t)(nowMs - startMs) >= (uint32_t)faultTimeMs;
}

bool controlFaultLatched() {
    return faultLatched;
}

void controlClearFault() {
    faultLatched = false;

    appsFaultStartMs = 0;
    tpsFaultStartMs = 0;
    trackingFaultStartMs = 0;

    resetPid();
    targetF = IDLE_POS;
    dacSetThrottlePct(ECU_DAC_IDLE_PCT);
}

void controlInit() {
    posF = IDLE_POS;
    targetF = IDLE_POS;

    integralError = 0.0f;
    prevPosF = IDLE_POS;
    dFilt = 0.0f;

    lastLoopUs = micros();

    appsFaultStartMs = 0;
    tpsFaultStartMs = 0;
    trackingFaultStartMs = 0;

    faultLatched = false;
    debugCounter = 0;

    dacSetThrottlePct(ECU_DAC_IDLE_PCT);
    motorDisable();
}

void controlTP() {
    const uint32_t nowUs = micros();

    if ((uint32_t)(nowUs - lastLoopUs) < LOOP_PERIOD_US) {
        return;
    }

    // Keep stable 1 kHz cadence. If we were delayed too much, resync.
    lastLoopUs += LOOP_PERIOD_US;
    if ((uint32_t)(nowUs - lastLoopUs) > 5000U) {
        lastLoopUs = nowUs;
    }

    if (faultLatched) {
        dacSetThrottlePct(ECU_DAC_IDLE_PCT);
        motorDisable();
        return;
    }

    ReadData appsData = readAppsPct();
    ReadData tpsData = readThrottlePct();

    if (persistentFault(!appsData.valid, appsFaultStartMs, APPS_FAULT_TIME_MS)) {
        latchFault();
        return;
    }

    if (persistentFault(!tpsData.valid, tpsFaultStartMs, TPS_FAULT_TIME_MS)) {
        latchFault();
        return;
    }

    // During short APPS glitch, command idle and do not trust the pedal value.
    if (!appsData.valid) {
        appsData.pos = IDLE_POS;
    }

    // During short TPS glitch, stop the motor until readings recover.
    if (!tpsData.valid) {
        dacSetThrottlePct(ECU_DAC_IDLE_PCT);
        motorStop();
        resetPid();
        return;
    }

    // Filter measured throttle position.
    posF += alpha * (tpsData.pos - posF);
    dacSetThrottlePct(posF);

    // Clamp command to valid throttle range.
    float wanted = clampf(appsData.pos, IDLE_POS, 100.0f);

    // Low-pass command, then slew-limit it.
    float targetNext = targetF + CMD_ALPHA * (wanted - targetF);
    targetF = slewLimit(targetF, targetNext, CMD_SLEW_RATE, LOOP_DT);

    const float error = targetF - posF;
    const float absError = fabsf(error);

    // Tracking fault: if target and actual are too far apart for too long.
    const bool trackingBad = absError > TRACKING_ERROR_LIMIT;
    if (persistentFault(trackingBad, trackingFaultStartMs, TRACKING_FAULT_TIME_MS)) {
        latchFault();
        return;
    }

    if (absError <= DEADBAND) {
        resetPid();
        motorStop();
        return;
    }

    // Derivative on measurement to avoid derivative kick on target changes.
    const float measurementDerivative = (posF - prevPosF) / LOOP_DT;
    dFilt += D_FILTER_ALPHA * (measurementDerivative - dFilt);

    const float P = Kp * error;

    // Conditional integration anti-windup.
    float candidateIntegral = integralError + error * LOOP_DT;
    candidateIntegral = clampf(candidateIntegral, -INTEGRAL_LIMIT, INTEGRAL_LIMIT);

    float I_candidate = Ki * candidateIntegral;
    float D = -Kd * dFilt;

    float unsatOutput = P + I_candidate + D;
    float output = clampf(unsatOutput, -OUTPUT_LIMIT, OUTPUT_LIMIT);

    const bool satHigh = unsatOutput > OUTPUT_LIMIT;
    const bool satLow = unsatOutput < -OUTPUT_LIMIT;

    if ((!satHigh && !satLow) ||
        (satHigh && error < 0.0f) ||
        (satLow && error > 0.0f)) {
        integralError = candidateIntegral;
    }

    const float I = Ki * integralError;
    output = clampf(P + I + D, -OUTPUT_LIMIT, OUTPUT_LIMIT);

    prevPosF = posF;

    int pwm = (int)fabsf(output);

    // Add minimum PWM only when movement is requested.
    if (pwm > 0 && pwm < PWM_MIN) {
        pwm = PWM_MIN;
    }

    const int pwmMax = (absError <= FAR_ZONE) ? PWM_NEAR_MAX : PWM_FAR;
    pwm = (int)clampf((float)pwm, 0.0f, (float)pwmMax);

    if (output > 0.0f) {
        motorOpen(pwm);
    } else {
        motorClose(pwm);
    }

    // Lightweight debug at ~20 Hz.
    debugCounter++;
    if (debugCounter >= 50) {
        debugCounter = 0;

        SerialUSB.print("APPS=");
        SerialUSB.print(appsData.pos);
        SerialUSB.print(" Target=");
        SerialUSB.print(targetF);
        SerialUSB.print(" TPS=");
        SerialUSB.print(posF);
        SerialUSB.print(" Err=");
        SerialUSB.print(error);
        SerialUSB.print(" PWM=");
        SerialUSB.print(pwm);
        SerialUSB.print(" Fault=");
        SerialUSB.println(faultLatched ? 1 : 0);
    }
}
