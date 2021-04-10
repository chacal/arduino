#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

struct Config {
  float    fanTurnOnTemp;
  float    fanTurnOffTemp;
  uint16_t maxFanSpeed;
  uint16_t minFanSpeed;
};

extern Config config;

void loadConfigFromFile();

void saveConfigToFile();

void updateConfigFromJson(const JsonVariant &json);

DynamicJsonDocument getConfigAsJson();

void printConfig();
