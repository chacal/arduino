#include <Arduino.h>
#include <ArduinoOTA.h>
#include <SetPoint.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include "utils.hpp"
#include "wifi.hpp"
#include "web_server.hpp"
#include "config.hpp"

#define FAN_MOSFET_GATE_PIN  D1
#define NTC_MEASUREMENT_PIN  A0
#define RESET_PIN            D7   // Pull this to GND during bootup to reset all settings (Wifi & configuration)
#define HOSTNAME             "esp-fan-controller"


WiFiManager   wifiManager;
SetPoint      setPoint;
bool fanOn                  = false;
volatile bool configUpdated = false;

void turnFanOn() {
  fanOn = true;
}

void turnFanOff() {
  fanOn = false;
}

void initializeSetpoint() {
  auto setpoint_temperature = (config.fanTurnOnTemp + config.fanTurnOffTemp) / 2;
  auto hysteresis           = (config.fanTurnOnTemp - config.fanTurnOffTemp) / 2;
  setPoint.begin(setpoint_temperature * 100, hysteresis * 100);
  setPoint.attach(RISING_EDGE, turnFanOn);
  setPoint.attach(FALLING_EDGE, turnFanOff);
  setPoint.update(0);
}

int calculateFanSpeed(double temperature) {
  auto     pwmUnitsPerOneDegreeTemp = (config.maxFanSpeed - config.minFanSpeed) / (config.fanTurnOnTemp - config.fanTurnOffTemp);
  uint16_t fanSpeed                 = round(config.minFanSpeed + (temperature - config.fanTurnOffTemp) * pwmUnitsPerOneDegreeTemp);
  return max(config.minFanSpeed, min(config.maxFanSpeed, fanSpeed));
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP-motor-fan-controller..");
  loadConfigFromFile();

  pinMode(LED_BUILTIN, OUTPUT);
  blink(1);

  resetAndRebootIfPinLow(RESET_PIN, wifiManager);

  pinMode(FAN_MOSFET_GATE_PIN, OUTPUT);
  digitalWrite(FAN_MOSFET_GATE_PIN, LOW);

  initializeSetpoint();

  connectWiFi(wifiManager, HOSTNAME);
  webServerInit([]() { configUpdated = true; });
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

  if (configUpdated) {
    configUpdated = false;
    turnFanOff();
    initializeSetpoint();
  }

  delay(1000);
}