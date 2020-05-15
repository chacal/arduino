#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

struct Config {
    String   mqttUrl;
    uint16_t mqttPort;
    boolean  verifyTlsCertificate;
    String   mqttUsername;
    String   mqttPassword;
    String   mqttTopic;
    String   hostname;
};

extern Config config;

void loadConfigFromFile();

void saveConfigToFile();

void updateConfigFromJson(const JsonVariant &doc);

void removeSavedConfig();

DynamicJsonDocument getConfigAsJson();

void printConfig();
