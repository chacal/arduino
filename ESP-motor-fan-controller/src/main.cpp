#include <Arduino.h>
#include <SetPoint.h>
#include "utils.hpp"

#define FAN_TURN_ON_TEMP     60
#define FAN_TURN_OFF_TEMP    45
#define FAN_MOSFET_GATE_PIN  D1
#define NTC_MEASUREMENT_PIN  A0

SetPoint setPoint;
bool fanOn = false;

void initializeSetpoint();

void turnFanOn() {
  fanOn = true;
  digitalWrite(FAN_MOSFET_GATE_PIN, HIGH);
}

void turnFanOff() {
  fanOn = false;
  digitalWrite(FAN_MOSFET_GATE_PIN, LOW);
}

void initializeSetpoint() {
  auto setpoint_temperature = (FAN_TURN_ON_TEMP + FAN_TURN_OFF_TEMP) / 2;
  auto hysteresis           = (FAN_TURN_ON_TEMP - FAN_TURN_OFF_TEMP) / 2;
  setPoint.begin(setpoint_temperature, hysteresis);
  setPoint.attach(RISING_EDGE, turnFanOn);
  setPoint.attach(FALLING_EDGE, turnFanOff);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(FAN_MOSFET_GATE_PIN, OUTPUT);
  digitalWrite(FAN_MOSFET_GATE_PIN, LOW);

  initializeSetpoint();

  if (measure_ntc_temp(NTC_MEASUREMENT_PIN) >= FAN_TURN_ON_TEMP) {
    turnFanOn();
  }
}

void loop() {
  auto temp = measure_ntc_temp(NTC_MEASUREMENT_PIN);
  setPoint.update(temp);

  Serial.println(String("Temp: ") + temp);

  blink(fanOn ? 3 : 1);
  delay(1000);
}