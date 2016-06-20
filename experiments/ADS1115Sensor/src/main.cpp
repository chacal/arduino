#include <Arduino.h>
#include "ADS1115.h"
#include <power.h>

#define SHUNT_RESISTOR_OHMS 1.6f

ADS1115 adc0;

void setup() {
  Serial.begin(57600);
  Wire.begin();

  adc0.initialize();
  adc0.setRate(ADS1115_RATE_8);
  adc0.setGain(ADS1115_PGA_0P256);
}

void loop() {
  adc0.triggerConversion();

  powerDown(WAKEUP_DELAY_250_MS);  // It takes 1/8th of a second (125ms) to measure, sleep for more than that

  unsigned long start = micros();

  int raw = adc0.getConversion(false);
  adc0.setMode(ADS1115_MODE_SINGLESHOT);  // This if needed for some reason after reading conversion, otherwise ADS1115 keeps consuming 11µA when supposed to be powered off

  float millivolts = raw * adc0.getMvPerCount();
  float currentMilliAmps = 1 / SHUNT_RESISTOR_OHMS * millivolts;

  unsigned long duration = micros() - start;

  Serial.println(String("Raw: ") + raw + "\tVoltage: " + millivolts + " mV\tCurrent: " + currentMilliAmps + " mA\t Duration: " + duration);
  Serial.flush();

  powerDown(WAKEUP_DELAY_1_S);
}
