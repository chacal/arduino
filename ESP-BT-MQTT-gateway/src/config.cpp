#include <FS.h>
#include <StreamPrint.h>

#include "config.hpp"

#define DEFAULT_MQTT_URL        "mqtts://mqtt-home.chacal.fi"
#define DEFAULT_MQTT_PORT       8883
#define DEFAULT_VERIFY_TLS_CERT true
#define DEFAULT_MQTT_USERNAME   "esp-sender"
#define DEFAULT_MQTT_PASSWORD   ""
#define DEFAULT_MQTT_TOPIC      "/test/bt-sensor-gw/1"
#define DEFAULT_HOST_NAME       "esp-bt-mqtt-test"

#define CONFIG_FILE_NAME       "config.json"

Config config = {
  DEFAULT_MQTT_URL,
  DEFAULT_MQTT_PORT,
  DEFAULT_VERIFY_TLS_CERT,
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

void setValueIfKeyExists(String *value, const JsonVariant &doc, const String &key) {
  if (hasStringKey(doc, key)) {
    *value = doc[key].as<String>();
  }
}

void updateConfigFromJson(const JsonVariant &doc) {
  setValueIfKeyExists(&config.mqttUrl, doc, "mqttUrl");
  setValueIfKeyExists(&config.mqttUsername, doc, "mqttUsername");
  setValueIfKeyExists(&config.mqttPassword, doc, "mqttPassword");
  setValueIfKeyExists(&config.mqttTopic, doc, "mqttTopic");
  setValueIfKeyExists(&config.hostname, doc, "hostname");

  config.mqttPort             = doc["mqttPort"] | config.mqttPort;
  config.verifyTlsCertificate = doc["verifyTlsCertificate"] | config.verifyTlsCertificate;
}

void removeSavedConfig() {
  if (SPIFFS.begin() && SPIFFS.exists(CONFIG_FILE_NAME)) {
    SPIFFS.remove(CONFIG_FILE_NAME);
  }
}

DynamicJsonDocument getConfigAsJson() {
  DynamicJsonDocument doc(512);
  doc["mqttUrl"]              = config.mqttUrl;
  doc["mqttPort"]             = config.mqttPort;
  doc["verifyTlsCertificate"] = config.verifyTlsCertificate;
  doc["mqttUsername"]         = config.mqttUsername;
  doc["mqttPassword"]         = config.mqttPassword;
  doc["mqttTopic"]            = config.mqttTopic;
  doc["hostname"]             = config.hostname;
  return doc;
}

void printConfig() {
  Serial << endl
         << "Using config:\n-------------" << endl;
  Serial << "MQTT url: \t\t" << config.mqttUrl << endl;
  Serial << "MQTT port: \t\t" << config.mqttPort << endl;
  Serial << "Verify TLS cert: \t" << (config.verifyTlsCertificate ? "true" : "false") << endl;
  Serial << "MQTT username: \t\t" << config.mqttUsername << endl;
  Serial << "MQTT password: \t\t" << config.mqttPassword.substring(0, 2) << "*********" << endl;
  Serial << "MQTT topic: \t\t" << config.mqttTopic << endl;
  Serial << "MDNS hostname: \t\t" << config.hostname << endl;
}
