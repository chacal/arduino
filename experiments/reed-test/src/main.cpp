#include <Arduino.h>
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT_PULLUP);
  mySwitch.enableTransmit(10);
}

void loop() {
  if(digitalRead(2) == LOW) {
    Serial.println("On!");
    mySwitch.send(12345, 24);
  } else {
    Serial.println("Off");
  }
  delay(1000);
}