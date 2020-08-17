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
#define FAN_MOSFET_GATE_PIN  D1
#define NTC_MEASUREMENT_PIN  A0
#define RESET_PIN            D7   // Pull this to GND during bootup to reset all settings (Wifi & configuration)
#define HOSTNAME             "esp-fan-controller"


WiFiManager wifiManager;
SetPoint    setPoint;
bool fanOn = false;

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

  if (measure_ntc_temp(NTC_MEASUREMENT_PIN) >= FAN_TURN_ON_TEMP) {
    turnFanOn();
  }
}

void loop() {
  auto temp = measure_ntc_temp(NTC_MEASUREMENT_PIN);
  setPoint.update(temp);

  Serial.println(String("Temp: ") + temp);
  webServerBroadcastWs(String(temp) + "°C, fan state: " + (fanOn ? "on" : "off"));

  blink(fanOn ? 3 : 1);
  ArduinoOTA.handle();

  delay(1000);
}