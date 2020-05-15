#include <FS.h>
#include <StreamPrint.h>

#include "config.hpp"

#define DEFAULT_MQTT_URL        "mqtts://mqtt-home.chacal.fi"
#define DEFAULT_MQTT_PORT       8883
#define DEFAULT_MQTT_USERNAME   "esp-sender"
#define DEFAULT_MQTT_PASSWORD   ""
#define DEFAULT_MQTT_TOPIC      "/test/bt-sensor-gw/1"
#define DEFAULT_HOST_NAME       "esp-bt-mqtt-test"

#define CONFIG_FILE_NAME       "config.json"

Config config = {
  DEFAULT_MQTT_URL,
  DEFAULT_MQTT_PORT,
  DEFAULT_MQTT_USERNAME,
  DEFAULT_MQTT_PASSWORD,
  DEFAULT_MQTT_TOPIC,
  DEFAULT_HOST_NAME
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

boolean hasStringKey(const JsonVariant &doc, const String &key) {
  return doc.containsKey(key) && doc[key].is<char *>();
}

void updateConfigFromJson(const JsonVariant &doc) {
  if (hasStringKey(doc, "mqttUrl")) {
    config.mqttUrl = doc["mqttUrl"].as<String>();
  }

  config.mqttPort = doc["mqttPort"] | config.mqttPort;

  if (hasStringKey(doc, "mqttUsername")) {
    config.mqttUsername = doc["mqttUsername"].as<String>();
  }

  if (hasStringKey(doc, "mqttPassword")) {
    config.mqttPassword = doc["mqttPassword"].as<String>();
  }

  if (hasStringKey(doc, "mqttTopic")) {
    config.mqttTopic = doc["mqttTopic"].as<String>();
  }

  if (hasStringKey(doc, "hostname")) {
    config.hostname = doc["hostname"].as<String>();
  }
}

void removeSavedConfig() {
  if (SPIFFS.begin() && SPIFFS.exists(CONFIG_FILE_NAME)) {
    SPIFFS.remove(CONFIG_FILE_NAME);
  }
}

DynamicJsonDocument getConfigAsJson() {
  DynamicJsonDocument doc(512);
  doc["mqttUrl"]      = config.mqttUrl;
  doc["mqttPort"]     = config.mqttPort;
  doc["mqttUsername"] = config.mqttUsername;
  doc["mqttPassword"] = config.mqttPassword;
  doc["mqttTopic"]    = config.mqttTopic;
  doc["hostname"]     = config.hostname;
  return doc;
}

void printConfig() {
  Serial << endl
         << "Using config:\n-------------" << endl;
  Serial << "MQTT url: \t" << config.mqttUrl << endl;
  Serial << "MQTT port: \t" << config.mqttPort << endl;
  Serial << "MQTT username: \t" << config.mqttUsername << endl;
  Serial << "MQTT password: \t" << config.mqttPassword.substring(0, 2) << "*********" << endl;
  Serial << "MQTT topic: \t" << config.mqttTopic << endl;
  Serial << "MDNS hostname: \t" << config.hostname << endl;
}
