#include <FS.h>
#include <StreamPrint.h>

#include "config.hpp"

#define DEFAULT_MQTT_SERVER     "mqtt-home.chacal.fi"
#define DEFAULT_MQTT_PORT       1883
#define DEFAULT_MQTT_TOPIC      "/test/bt-sensor-gw/1"

#define CONFIG_FILE_NAME       "config.json"

Config config = {
    DEFAULT_MQTT_SERVER,
    DEFAULT_MQTT_PORT,
    DEFAULT_MQTT_TOPIC
};

void loadConfigFromFile() {
  SPIFFS.begin();
  File                    config_file = SPIFFS.open(CONFIG_FILE_NAME, "r");
  StaticJsonDocument<512> doc;

  DeserializationError error = deserializeJson(doc, config_file);
  if (error) {
    Serial.println("Failed to read config file, using default configuration");
    saveConfigToFile();
  } else {
    updateConfigFromJson(doc.as<JsonVariant>());
  }

  config_file.close();
}

void saveConfigToFile() {
  SPIFFS.begin();
  auto file = SPIFFS.open(CONFIG_FILE_NAME, "w");

  if (!file) {
    Serial.println("Failed to create file!");
    return;
  }

  serializeJson(getConfigAsJson(), file);
  file.close();
}

void updateConfigFromJson(const JsonVariant &doc) {
  if (doc.containsKey("mqttServer") && doc["mqttServer"].is<char *>()) {
    config.mqttServer = doc["mqttServer"].as<String>();
  }

  config.mqttPort = doc["mqttPort"] | config.mqttPort;

  if (doc.containsKey("mqttTopic") && doc["mqttTopic"].is<char *>()) {
    config.mqttTopic = doc["mqttTopic"].as<String>();
  }
}

void removeSavedConfig() {
  if (SPIFFS.begin() && SPIFFS.exists(CONFIG_FILE_NAME)) {
    SPIFFS.remove(CONFIG_FILE_NAME);
  }
}

DynamicJsonDocument getConfigAsJson() {
  DynamicJsonDocument doc(512);
  doc["mqttServer"] = config.mqttServer;
  doc["mqttPort"]   = config.mqttPort;
  doc["mqttTopic"]  = config.mqttTopic;
  return doc;
}

void printConfig() {
  Serial << endl
         << "Using config:\n-------------" << endl;
  Serial << "MQTT server: \t" << config.mqttServer << endl;
  Serial << "MQTT port: \t" << config.mqttPort << endl;
  Serial << "MQTT topic: \t" << config.mqttTopic << endl;
}
