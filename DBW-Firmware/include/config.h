#pragma once

// TPS calibration
extern int tps1Min, tps1Max;
extern int tps2Min, tps2Max;

// Control parameters
extern float IDLE_POS;
extern float FAR_ZONE;
extern float DEADBAND;

// PWM limits
extern int PWM_MIN;
extern int PWM_NEAR_MAX;
extern int PWM_FAR;

// Filter
extern float alpha;

// PID parameters
extern float Kp;
extern float Ki;
extern float Kd;

// Command (setpoint) filter
extern float CMD_ALPHA;      // smoothing factor for incoming command (0..1)
extern float CMD_SLEW_RATE;  // maximum change in command (percent per second)
