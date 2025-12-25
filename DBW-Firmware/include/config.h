#pragma once

// TPS calibration
extern int tps1Min, tps1Max;
extern int tps2Min, tps2Max;

// Control parameters
extern const float IDLE_POS;
extern const float FAR_ZONE;
extern const float DEADBAND;

// PWM limits
extern const int PWM_MIN;
extern const int PWM_NEAR_MAX;
extern const int PWM_FAR;

// Filter
extern float alpha;
