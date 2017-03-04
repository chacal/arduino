#pragma once

struct MqttConfiguration {
  char server[100];
  char topic[100];
  char port[6];

  MqttConfiguration(const char *defaultServer, const char *defaultTopic, const char *defaultPort = "1883") {
    strcpy(server, defaultServer);
    strcpy(topic, defaultTopic);
    strcpy(port, defaultPort);
  }
};

class ConfigSaver {
public:
  bool loadConfiguration(MqttConfiguration &conf);
  void saveConfiguration(MqttConfiguration &conf);
};
