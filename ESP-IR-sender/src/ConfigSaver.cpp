#include <FS.h>
#include <ArduinoJson.h>
#include "ConfigSaver.h"
#include "StreamPrint.h"

#define CONF_FILE "/config.json"

void ConfigSaver::loadConfiguration(MqttConfiguration &conf) {
  if(SPIFFS.begin() && SPIFFS.exists(CONF_FILE)) {
    File configFile = SPIFFS.open(CONF_FILE, "r");

    size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);

    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.parseObject(buf.get());
    if(json.success()) {
      strcpy(conf.server, json["mqtt_server"]);
      strcpy(conf.topic, json["mqtt_topic"]);
      strcpy(conf.port, json["mqtt_port"]);

      Serial << "Read JSON configuration: ";
      json.printTo(Serial);
      Serial << endl;
    } else {
      Serial << "Failed to parse json config" << endl;
    }
  } else {
    Serial << "Failed to read config file." << endl;
  }
}

void ConfigSaver::saveConfiguration(MqttConfiguration &conf) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();
  json["mqtt_server"] = conf.server;
  json["mqtt_topic"] = conf.topic;
  json["mqtt_port"] = conf.port;

  Serial << "Saving config: ";
  json.printTo(Serial);
  Serial << endl;

  File configFile = SPIFFS.open(CONF_FILE, "w");
  if(!configFile) {
    Serial << "Failed to open config file for writing" << endl;
  }

  json.printTo(configFile);
  configFile.close();
}