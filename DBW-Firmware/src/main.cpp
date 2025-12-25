#include <Arduino.h>
#include "adc.h"
#include "motor.h"
#include "control.h"

int cmd = 0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  adcInit();
  motorInit();
  controlInit();
}

void loop() {
  if (Serial.available()) {
    cmd = constrain(Serial.parseInt(), 0, 100);
  }

  controlUpdate(cmd);
  // delay(5);
}
