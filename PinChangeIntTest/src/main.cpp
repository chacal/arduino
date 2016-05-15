#include <Arduino.h>
#include <power.h>

#include "main.h"
#include "buttons.h"

/*
 * This sketch uses less that 1µA when in sleep, but Arduino is woken up when the buttons are pressed
 */

unsigned long wakeUpTime = 0;


void setup() {
  Serial.begin(115200);
  setupPinChangeInterrupts();
  setupButtonCallbacks();
  goToSleep();
}

void loop() {
  updateButtonStates();

  if(hasMinAwakeTimeElapsed() && noButtonsPressed()) {
    goToSleep();
  }
}


bool hasMinAwakeTimeElapsed() { return millis() - wakeUpTime > MIN_AWAKE_TIME_MS; }

void goToSleep() {
  Serial.println("Sleeping");
  Serial.flush();
  powerDown();
  wakeUpTime = millis();
}
