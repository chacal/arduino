#include <FS.h>
#include <ArduinoJson.h>
#include "ConfigSaver.h"
#include "StreamPrint.h"

bool ConfigSaver::loadConfiguration(MqttConfiguration &conf) {
  if(SPIFFS.begin()) {
    Serial << "mounted file system" << endl;
    if(SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial << "reading config file" << endl;

      File configFile = SPIFFS.open("/config.json", "r");
      if(configFile) {
        Serial << "opened config file" << endl;
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if(json.success()) {
          Serial << "\nparsed json" << endl;

          strcpy(conf.server, json["mqtt_server"]);
          strcpy(conf.topic, json["mqtt_topic"]);
          strcpy(conf.port, json["mqtt_port"]);

        } else {
          Serial << "failed to load json config" << endl;
        }
      }
    } else {
      Serial << "Config file not found!" << endl;
    }
  } else {
    Serial << "failed to mount FS" << endl;
  }
}

void ConfigSaver::saveConfiguration(MqttConfiguration &conf) {
  Serial << "saving config" << endl;
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();
  json["mqtt_server"] = conf.server;
  json["mqtt_topic"] = conf.topic;
  json["mqtt_port"] = conf.port;

  File configFile = SPIFFS.open("/config.json", "w");
  if(!configFile) {
    Serial << "failed to open config file for writing" << endl;
  }

  json.printTo(Serial);
  json.printTo(configFile);
  configFile.close();
}