#include <Arduino.h>
#include <ArduinoOTA.h>
#include <SetPoint.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include "utils.hpp"
#include "wifi.hpp"
#include "web_server.hpp"

#define FAN_TURN_ON_TEMP     60
#define FAN_TURN_OFF_TEMP    45
#define MAX_FAN_SPEED      1023
#define MIN_FAN_SPEED       800
#define FAN_MOSFET_GATE_PIN  D1
#define NTC_MEASUREMENT_PIN  A0
#define RESET_PIN            D7   // Pull this to GND during bootup to reset all settings (Wifi & configuration)
#define HOSTNAME             "esp-fan-controller"


WiFiManager wifiManager;
SetPoint    setPoint;
bool fanOn = false;

void turnFanOn() {
  fanOn = true;
}

void turnFanOff() {
  fanOn = false;
}

void initializeSetpoint() {
  auto setpoint_temperature = ((double) FAN_TURN_ON_TEMP + FAN_TURN_OFF_TEMP) / 2;
  auto hysteresis           = ((double) FAN_TURN_ON_TEMP - FAN_TURN_OFF_TEMP) / 2;
  setPoint.begin(setpoint_temperature * 100, hysteresis * 100);
  setPoint.attach(RISING_EDGE, turnFanOn);
  setPoint.attach(FALLING_EDGE, turnFanOff);
  setPoint.update(0);
}

int calculateFanSpeed(double temperature) {
  auto pwmUnitsPerOneDegreeTemp = ((double) MAX_FAN_SPEED - MIN_FAN_SPEED) / (FAN_TURN_ON_TEMP - FAN_TURN_OFF_TEMP);
  int  fanSpeed                 = round(MIN_FAN_SPEED + (temperature - FAN_TURN_OFF_TEMP) * pwmUnitsPerOneDegreeTemp);
  return max(MIN_FAN_SPEED, min(MAX_FAN_SPEED, fanSpeed));
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP-motor-fan-controller..");

  pinMode(LED_BUILTIN, OUTPUT);
  blink(1);

  resetAndRebootIfPinLow(RESET_PIN, wifiManager);

  pinMode(FAN_MOSFET_GATE_PIN, OUTPUT);
  digitalWrite(FAN_MOSFET_GATE_PIN, LOW);

  initializeSetpoint();

  connectWiFi(wifiManager, HOSTNAME);
  webServerInit();
  MDNS.begin(HOSTNAME);
  ArduinoOTA.begin();
}

void loop() {
  auto temp = measure_ntc_temp(NTC_MEASUREMENT_PIN);
  setPoint.update(temp * 100);

  if (fanOn) {
    analogWrite(FAN_MOSFET_GATE_PIN, calculateFanSpeed(temp));
  } else {
    digitalWrite(FAN_MOSFET_GATE_PIN, LOW);
  }

  Serial.println(String("Temp: ") + temp);
  webServerBroadcastWs(String(temp) + "°C, fan state: " + (fanOn ? "on, fan speed: " + String(calculateFanSpeed(temp)) : "off"));

  blink(fanOn ? 3 : 1);
  ArduinoOTA.handle();

  delay(1000);
}