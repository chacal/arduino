#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

struct Config {
    String   mqttServer;
    uint16_t mqttPort;
    String   mqttTopic;
};

extern Config config;

void loadConfigFromFile();

void saveConfigToFile();

void updateConfigFromJson(const JsonVariant &doc);

void removeSavedConfig();

DynamicJsonDocument getConfigAsJson();

void printConfig();
