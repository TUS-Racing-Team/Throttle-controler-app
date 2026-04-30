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
static constexpr uint16_t DEBUG_PERIOD_LOOPS = 100;
static constexpr uint8_t DEBUG_RAW_EVERY_LINES = 10;
static constexpr float PWM_MIN_ERROR_PCT = 3.0f;
static constexpr float IDLE_HOLD_TARGET_MARGIN_PCT = 2.0f;
static constexpr int IDLE_HOLD_PWM = 30;
static constexpr float FSG_IDLE_TOLERANCE_PCT = 5.0f;
static constexpr uint32_t FSG_IDLE_CONFIRM_MS = 1000;

static float posF = 0.0f;
static float targetF = 0.0f;

static float integralError = 0.0f;
static float prevPosF = 0.0f;
static float dFilt = 0.0f;

static uint32_t lastLoopUs = 0;
static uint32_t appsFaultStartMs = 0;
static uint32_t tpsFaultStartMs = 0;
static uint32_t trackingFaultStartMs = 0;
static uint32_t faultIdleStartMs = 0;

static bool faultLatched = false;
static bool faultIdleConfirmed = true;
static ControlFaultReason faultReason = CONTROL_FAULT_NONE;

static uint16_t debugCounter = 0;
static uint8_t debugLineCounter = 0;
static int lastPwm = 0;
static int stictionBoostDirection = 0;

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
    stictionBoostDirection = 0;
}

static void printPctOrInvalid(const char* label, const ReadData& data, bool includeRaw) {
    Serial.print(label);
    if (data.valid) {
        Serial.print(data.pos);
    } else {
        Serial.print("INVALID(");
        Serial.print(data.pos);
        Serial.print(")");
    }

    if (!includeRaw) {
        return;
    }

    Serial.print("[r1=");
    Serial.print(data.raw1);
    Serial.print(" r2=");
    Serial.print(data.raw2);
    Serial.print(" p1=");
    Serial.print(data.pos1);
    Serial.print(" p2=");
    Serial.print(data.pos2);
    Serial.print(" rawOk=");
    Serial.print(data.rawOk ? 1 : 0);
    Serial.print(" corr=");
    Serial.print(data.correlationOk ? 1 : 0);
    Serial.print("]");
}

static void printControlDebug(const ReadData& appsData, const ReadData& tpsData) {
    debugCounter++;
    if (debugCounter < DEBUG_PERIOD_LOOPS) {
        return;
    }
    debugCounter = 0;

    debugLineCounter++;
    bool includeRaw = debugLineCounter >= DEBUG_RAW_EVERY_LINES;
    includeRaw = includeRaw || !appsData.valid || !tpsData.valid || faultLatched;
    if (includeRaw) {
        debugLineCounter = 0;
    }

    printPctOrInvalid("APPS=", appsData, includeRaw);
    Serial.print(" Target=");
    Serial.print(targetF);
    printPctOrInvalid(" TPS=", tpsData, includeRaw);
    Serial.print(" PosF=");
    Serial.print(posF);
    Serial.print(" Err=");
    Serial.print(targetF - posF);
    Serial.print(" PWM=");
    Serial.print(lastPwm);
    Serial.print(" Fault=");
    Serial.print(faultLatched ? 1 : 0);
    Serial.print(" Reason=");
    Serial.println(controlFaultReasonText());
}

static bool throttleIsIdle(float throttlePct) {
    return fabsf(throttlePct - IDLE_POS) <= FSG_IDLE_TOLERANCE_PCT;
}

static void latchFault(ControlFaultReason reason) {
    if (!faultLatched) {
        faultReason = reason;
        faultIdleStartMs = 0;
        faultIdleConfirmed = false;
    }

    faultLatched = true;
    resetPid();
    targetF = IDLE_POS;
    lastPwm = 0;
    dacSetThrottlePct(ECU_DAC_IDLE_PCT);

    // Real fault behavior: disable driver enable pins.
    motorDisable();
}

static void monitorFaultedThrottle(const ReadData& tpsData) {
    if (tpsData.valid) {
        posF = tpsData.pos;
        prevPosF = posF;
    }

    if (tpsData.valid && throttleIsIdle(tpsData.pos)) {
        const uint32_t nowMs = millis();

        if (faultIdleStartMs == 0) {
            faultIdleStartMs = nowMs;
        }

        if ((uint32_t)(nowMs - faultIdleStartMs) >= FSG_IDLE_CONFIRM_MS) {
            faultIdleConfirmed = true;
        }
    } else {
        faultIdleStartMs = 0;
        faultIdleConfirmed = false;
    }

    dacSetThrottlePct(ECU_DAC_IDLE_PCT);
    lastPwm = 0;
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

ControlFaultReason controlFaultReason() {
    return faultReason;
}

const char* controlFaultReasonText() {
    switch (faultReason) {
        case CONTROL_FAULT_APPS:
            return "APPS";
        case CONTROL_FAULT_TPS:
            return "TPS";
        case CONTROL_FAULT_TRACKING:
            return "TRACKING";
        case CONTROL_FAULT_NONE:
        default:
            return "NONE";
    }
}

bool controlClearFault() {
    ReadData appsData = readAppsPct();
    ReadData tpsData = readThrottlePct();

    if (!appsData.valid || !tpsData.valid || !throttleIsIdle(tpsData.pos)) {
        return false;
    }

    if (faultLatched && !faultIdleConfirmed) {
        return false;
    }

    faultLatched = false;
    faultReason = CONTROL_FAULT_NONE;
    faultIdleStartMs = 0;
    faultIdleConfirmed = true;

    appsFaultStartMs = 0;
    tpsFaultStartMs = 0;
    trackingFaultStartMs = 0;

    resetPid();
    targetF = IDLE_POS;
    dacSetThrottlePct(ECU_DAC_IDLE_PCT);

    return true;
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
    faultIdleStartMs = 0;

    faultLatched = false;
    faultIdleConfirmed = true;
    faultReason = CONTROL_FAULT_NONE;
    debugCounter = 0;
    debugLineCounter = 0;
    lastPwm = 0;
    stictionBoostDirection = 0;

    dacSetThrottlePct(ECU_DAC_IDLE_PCT);
    motorDisable();
}

void controlTP() {
    analogWrite(3, 150);
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
        ReadData appsData = readAppsPct();
        ReadData tpsData = readThrottlePct();
        monitorFaultedThrottle(tpsData);
        printControlDebug(appsData, tpsData);
        return;
    }

    ReadData appsData = readAppsPct();
    ReadData tpsData = readThrottlePct();
    printControlDebug(appsData, tpsData);

    if (persistentFault(!appsData.valid, appsFaultStartMs, APPS_FAULT_TIME_MS)) {
        latchFault(CONTROL_FAULT_APPS);
        return;
    }

    if (persistentFault(!tpsData.valid, tpsFaultStartMs, TPS_FAULT_TIME_MS)) {
        latchFault(CONTROL_FAULT_TPS);
        return;
    }

    // During short APPS glitch, command idle and do not trust the pedal value.
    if (!appsData.valid) {
        appsData.pos = IDLE_POS;
    }

    // During short TPS glitch, stop the motor until readings recover.
    if (!tpsData.valid) {
        dacSetThrottlePct(ECU_DAC_IDLE_PCT);
        lastPwm = 0;
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
    const bool idleTarget = targetF <= (IDLE_POS + IDLE_HOLD_TARGET_MARGIN_PCT);

    // Tracking fault: if target and actual are too far apart for too long.
    const bool trackingBad = absError > TRACKING_ERROR_LIMIT;
    if (persistentFault(trackingBad, trackingFaultStartMs, TRACKING_FAULT_TIME_MS)) {
        latchFault(CONTROL_FAULT_TRACKING);
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

    if (idleTarget && absError <= DEADBAND) {
        stictionBoostDirection = 0;
        prevPosF = posF;

        if (error > 0.0f) {
            motorOpen(IDLE_HOLD_PWM);
            lastPwm = IDLE_HOLD_PWM;
        } else {
            motorStop();
            lastPwm = 0;
        }

        return;
    }

    if (absError <= DEADBAND) {
        lastPwm = 0;
        stictionBoostDirection = 0;
        motorStop();
        prevPosF = posF;
        return;
    }

    const int errorDirection = (error > 0.0f) ? 1 : -1;
    if (stictionBoostDirection != 0 && stictionBoostDirection != errorDirection) {
        stictionBoostDirection = 0;
    }

    if (idleTarget && error < 0.0f) {
        pwm = 0;
        stictionBoostDirection = 0;
    }

    if (absError >= PWM_MIN_ERROR_PCT && (!idleTarget || error > 0.0f)) {
        stictionBoostDirection = errorDirection;
    }

    // Static friction needs a real kick to start moving. Keep that boost active
    // until the throttle enters the deadband, otherwise it stalls below idle.
    if (pwm > 0 && pwm < PWM_MIN && stictionBoostDirection == errorDirection) {
        pwm = PWM_MIN;
    }

    const int pwmMax = (absError <= FAR_ZONE) ? PWM_NEAR_MAX : PWM_FAR;
    pwm = (int)clampf((float)pwm, 0.0f, (float)pwmMax);

    if (pwm == 0) {
        motorStop();
        lastPwm = 0;
        prevPosF = posF;
        return;
    }

    if (output > 0.0f) {
        motorOpen(pwm);
    } else {
        motorClose(pwm);
    }

    lastPwm = pwm;
}
