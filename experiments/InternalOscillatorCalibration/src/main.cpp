#include "Arduino.h"

int initialValue = OSCCAL;

void setup() {
  OSCCAL = 152;
  Serial.begin(115200);
  Serial.print("Factory OSCCAL value: ");
  Serial.println(initialValue);

  delay(1000);

  int start = initialValue - 30;
  int end = initialValue + 30;
  for(int i = start; i <= end; i++) {
    OSCCAL = i;
    Serial.print("Trying value ");
    Serial.println(OSCCAL);
    delay(30);
  }

  OSCCAL = 152;
  Serial.print("Trying value ");
  Serial.println(OSCCAL);

  Serial.println("Done!");
  pinMode(13, OUTPUT);
}

void loop() {
  digitalWrite(13,!digitalRead(13));// Turn the LED from off to on, or on to off
  delay(10000);          // Wait for 1 second (1000 milliseconds)
}
