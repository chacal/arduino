#include "config.hpp"
#include "LittleFS.h"

#define FAN_TURN_ON_TEMP     60
#define FAN_TURN_OFF_TEMP    45
#define MAX_FAN_SPEED      1023
#define MIN_FAN_SPEED       800

#define CONFIG_FILE_NAME       "config.json"

Config config = {
  FAN_TURN_ON_TEMP,
  FAN_TURN_OFF_TEMP,
  MAX_FAN_SPEED,
  MIN_FAN_SPEED
};

void loadConfigFromFile() {
  LittleFS.begin();
  File                    config_file = LittleFS.open(CONFIG_FILE_NAME, "r");
  StaticJsonDocument<512> doc;

  DeserializationError error = deserializeJson(doc, config_file);
  if (error) {
    Serial.println("Failed to read config file, using default configuration. Error was: " + String(error.c_str()));
    printConfig();
    saveConfigToFile();
  } else {
    updateConfigFromJson(doc.as<JsonVariant>());
  }

  config_file.close();
}

void saveConfigToFile() {
  LittleFS.begin();
  auto file = LittleFS.open(CONFIG_FILE_NAME, "w");

  if (!file) {
    Serial.println("Failed to create file!");
    return;
  }

  serializeJson(getConfigAsJson(), file);
  file.close();
}

void updateConfigFromJson(const JsonVariant &json) {
  config.fanTurnOnTemp  = json["fanTurnOnTemp"] | config.fanTurnOnTemp;
  config.fanTurnOffTemp = json["fanTurnOffTemp"] | config.fanTurnOffTemp;
  config.maxFanSpeed    = json["maxFanSpeed"] | config.maxFanSpeed;
  config.minFanSpeed    = json["minFanSpeed"] | config.minFanSpeed;

  printConfig();
}

DynamicJsonDocument getConfigAsJson() {
  DynamicJsonDocument doc(512);
  doc["fanTurnOnTemp"]  = config.fanTurnOnTemp;
  doc["fanTurnOffTemp"] = config.fanTurnOffTemp;
  doc["maxFanSpeed"]    = config.maxFanSpeed;
  doc["minFanSpeed"]    = config.minFanSpeed;
  return doc;
}

void printConfig() {
  Serial.println("Using config:\n-------------");
  Serial.printf("Turn on temp: \t%.2f\n", config.fanTurnOnTemp);
  Serial.printf("Turn off temp: \t%.2f\n", config.fanTurnOffTemp);
  Serial.printf("Max fan speed: \t%u\n", config.maxFanSpeed);
  Serial.printf("Min fan speed: \t%u\n", config.minFanSpeed);
}
