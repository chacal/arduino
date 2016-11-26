/*
 * See connections e.g. http://bildr.org/2012/03/rfp30n06le-arduino/
 */

#include <Arduino.h>

#define GATE_PIN 3

void setup()
{
  // initialize LED digital pin as an output.
  pinMode(GATE_PIN, OUTPUT);
  digitalWrite(GATE_PIN, HIGH);
}

void loop()
{
  // analogWrite(GATE_PIN, 40);
  // delay(5000);
  // analogWrite(GATE_PIN, 255);
  // delay(5000);

  for(int i = 0; i<360; i++){
    //convert 0-360 angle to radian (needed for sin function)
    float rad = DEG_TO_RAD * i;

    //calculate sin of angle as number between 0 and 255
    int sinOut = constrain((sin(rad) * 128) + 128, 0, 255);

    analogWrite(GATE_PIN, sinOut);

    delay(15);
  }
}
