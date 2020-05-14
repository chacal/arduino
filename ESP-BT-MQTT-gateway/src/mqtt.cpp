#include <StreamPrint.h>
#include "config.hpp"

#include "mqtt.hpp"


void connectMQTT(PubSubClient &mqttClient, Client &client) {
  mqttClient.setServer(config.mqttServer.c_str(), config.mqttPort);
  mqttClient.setClient(client);

  // Loop until we're connected
  while (!mqttClient.connected()) {
    String clientId = "ESP-BT-MQTT-" + String(random(0xffff), HEX);

    Serial << "Connecting to " << config.mqttServer << ":" << config.mqttPort << " as " << config.mqttUsername << "@" << clientId << endl;

    auto user = config.mqttUsername.length() > 0 ? config.mqttUsername.c_str() : NULL;
    auto pass = config.mqttPassword.length() > 0 ? config.mqttPassword.c_str() : NULL;

    if (mqttClient.connect(clientId.c_str(), user, pass)) {
      Serial << "MQTT connected as user " << config.mqttUsername << endl;
    } else {
      Serial << "MQTT connection failed, rc=" << mqttClient.state() << ", trying again in 5 seconds" << endl;
      delay(5000);
    }
  }
}

void publishToMqtt(PubSubClient &mqttClient, char *str) {
  mqttClient.publish((config.mqttTopic + "/value").c_str(), str, false);
}

