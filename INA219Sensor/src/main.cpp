#include <Arduino.h>
#include <Wire.h>
#include <INA219.h>
#include "main.hpp"
#include <power.h>

INA219 ina219;


void setup()
{
  Serial.begin(57600);
  initializeINA219();
}

void loop()
{
  configureINA219(TRIGGER_SHUNT);
  powerDown(WAKEUP_DELAY_64_MS);  // It takes ~34 ms to measure shunt with 64 samples

  Serial.print("raw shunt voltage: ");
  Serial.println(ina219.shuntVoltageRaw());

  Serial.print("shunt voltage: ");
  Serial.print(ina219.shuntVoltage() * 1000, 4);
  Serial.println(" mV");

  Serial.print("shunt current: ");
  Serial.print(ina219.shuntCurrent() * 1000, 4);
  Serial.println(" mA");

  Serial.println(" ");
  Serial.flush();

  configureINA219(POWER_DOWN);
  powerDown(WAKEUP_DELAY_2_S);
}


void initializeINA219() {
  ina219.begin(0x40);
  configureINA219(TRIGGER_SHUNT);
  ina219.calibrate(0.0005, 0.05, 15, 100);     // 5mΩ shunt, 40mV max shunt voltage, max bus voltage, max current in shunt
}

void configureINA219(Ina219Mode mode) {
  ina219.configure(0, 0, 3, 0xE, mode);        // 0-16V bus voltage range, ±40mV scale, 12-bit bus ADC, 12-bit + 64 sample avg shunt ADC, measure shunt only triggered / power down
}
