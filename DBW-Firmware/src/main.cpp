#include <Arduino.h>
#include "adc.h"
#include "motor.h"
#include "control.h"
#include "../include/comm/comm.h"

int cmd = 0;

// Forward declarations for comm interface (header include may be inconsistent in build)
void commInit();
void commUpdate();
bool commIsActive();

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  adcInit();
  motorInit();
  controlInit();
  commInit();
}

void loop() {
  // if (Serial.available()) {
  //   cmd = constrain(Serial.parseInt(), 0, 100);
  // }

  controlUpdate(0);
  
  if (commIsActive()) {
    commUpdate();
  }
}
