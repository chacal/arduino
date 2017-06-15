#include <Arduino.h>
#include <RCSwitch.h>
#include "power.h"
#include <EnableInterrupt.h>

#define RADIO_DATA_PIN   4
#define TRIGGER_PIN      3

#define SWITCH_FAMILY   'k'
#define SWITCH_GROUP     1
#define SWITCH_DEVICE    1

RCSwitch mySwitch = RCSwitch();

volatile bool interrupted = false;

void setup() {
  mySwitch.enableTransmit(RADIO_DATA_PIN);
  pinMode(TRIGGER_PIN, INPUT);
  enableInterrupt(TRIGGER_PIN, []() -> void{ interrupted = true; }, CHANGE);
}

void loop() {
  powerDown();

  if(interrupted) {
    if(digitalRead(TRIGGER_PIN) == HIGH) {
      delay(20);
      if(digitalRead(TRIGGER_PIN) == HIGH) {
        mySwitch.switchOn(SWITCH_FAMILY, SWITCH_GROUP, SWITCH_DEVICE);
      }
    } else {
      delay(20);
      if(digitalRead(TRIGGER_PIN) == LOW) {
        mySwitch.switchOff(SWITCH_FAMILY, SWITCH_GROUP, SWITCH_DEVICE);
      }
    }
    interrupted = false;
  }
}
