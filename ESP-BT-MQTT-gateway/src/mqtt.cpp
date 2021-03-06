#include <StreamPrint.h>
#include "config.hpp"
#include "util.hpp"

#include "mqtt.hpp"


void connectMQTT(PubSubClient &mqttClient, Client &client) {
  String address = addressFromMqttUrl(config.mqttUrl);

  mqttClient.setServer(address.c_str(), config.mqttPort);
  mqttClient.setClient(client);

  uint8_t retriesLeft = 60;  // Try to establish connection for 5 minutes before rebooting
  // Loop until we're connected or timed out
  while (!mqttClient.connected() && retriesLeft > 0) {
    String clientId = "ESP-BT-MQTT-" + String(random(0xffff), HEX);

    Serial << "Connecting to " << config.mqttUrl << ":" << config.mqttPort << " as " << config.mqttUsername << "@" << clientId << endl;

    auto user = config.mqttUsername.length() > 0 ? config.mqttUsername.c_str() : NULL;
    auto pass = config.mqttPassword.length() > 0 ? config.mqttPassword.c_str() : NULL;

    if (mqttClient.connect(clientId.c_str(), user, pass)) {
      Serial << "MQTT connected as user " << config.mqttUsername << endl;
    } else {
      Serial << "MQTT connection failed, rc=" << mqttClient.state() << ", trying again in 5 seconds" << endl;
      delay(5000);
    }

    retriesLeft--;
  }

  if (retriesLeft == 0) {
    Serial << "MQTT connection timed out. Rebooting.." << endl;
    ESP.restart();
  }
}

void publishToMqtt(PubSubClient &mqttClient, char *str) {
  mqttClient.publish((config.mqttTopic + "/value").c_str(), str, false);
}

