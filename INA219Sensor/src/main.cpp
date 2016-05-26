#include <Arduino.h>
#include <Wire.h>
#include <INA219.h>

INA219 ina219;


void setup()
{
  Serial.begin(57600);
  ina219.begin(0x40);
  ina219.configure(0, 0, 3, 3, 1);
  ina219.calibrate(0.1, 0.04, 6, 0.4);
}

void loop()
{

  ina219.configure(0, 0, 3, 3, 1);
  delay(1);

  Serial.println("******************");

  Serial.print("raw shunt voltage: ");
  Serial.println(ina219.shuntVoltageRaw());

  Serial.print("raw bus voltage:   ");
  Serial.println(ina219.busVoltageRaw());

  Serial.println("--");

  Serial.print("shunt voltage: ");
  Serial.print(ina219.shuntVoltage() * 1000, 4);
  Serial.println(" mV");

  Serial.print("shunt current: ");
  Serial.print(ina219.shuntCurrent() * 1000, 4);
  Serial.println(" mA");

  Serial.print("bus voltage:   ");
  Serial.print(ina219.busVoltage(), 4);
  Serial.println(" V");

  Serial.print("bus power:     ");
  Serial.print(ina219.busPower() * 1000, 4);
  Serial.println(" mW");

  Serial.println(" ");
  Serial.println(" ");

  ina219.configure(0, 0, 3, 3, 0);


  delay(5000);

}
