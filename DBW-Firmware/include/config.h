#pragma once

// =====================
// Sensor calibration
// =====================

// APPS calibration
extern int app1Min, app1Max;
extern int app2Min, app2Max;

// TPS calibration
extern int tps1Min, tps1Max;
extern int tps2Min, tps2Max;

// =====================
// Control parameters
// =====================

extern float IDLE_POS;
extern float FAR_ZONE;
extern float DEADBAND;

// PWM limits
extern int PWM_MIN;
extern int PWM_NEAR_MAX;
extern int PWM_FAR;

// Low-pass filter for measured TPS
extern float alpha;

// PID parameters
extern float Kp;
extern float Ki;
extern float Kd;

// Command/setpoint handling
extern float CMD_ALPHA;
extern float CMD_SLEW_RATE;

// Fault handling
extern float APPS_FAULT_TIME_MS;
extern float TPS_FAULT_TIME_MS;
extern float TRACKING_FAULT_TIME_MS;
extern float TRACKING_ERROR_LIMIT;

// PID safety limits
extern float INTEGRAL_LIMIT;
extern float D_FILTER_ALPHA;
extern float OUTPUT_LIMIT;

// =====================
// ECU analog output
// =====================

extern float ECU_DAC_IDLE_PCT;
extern float ECU_DAC_MIN_RAW;
extern float ECU_DAC_MAX_RAW;

// =====================
// Link ECU CAN
// =====================

extern int LINK_ECU_CAN_BITRATE;
extern int LINK_ECU_CAN_ID;
extern int LINK_ECU_CAN_TIMEOUT_MS;
