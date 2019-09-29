#include <Arduino.h>

#include "wifi.hpp"

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nStarting ESP blink detector");
  connectWifi();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
