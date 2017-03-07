#include "NetworkConnectionManager.h"


void connectWiFi(WiFiManager &wifiManager, MqttConfiguration &mqttConfig, void (*saveConfigCallback)()) {
  Serial << "Connecting to WiFi.." << endl;

  WiFiManagerParameter serverParam("server", "MQTT server", mqttConfig.server, 100);
  WiFiManagerParameter portParam("port", "MQTT port", mqttConfig.port, 6);
  WiFiManagerParameter topicRootParam("topic_root", "MQTT topic root", mqttConfig.topicRoot, 100);

  wifiManager.addParameter(&serverParam);
  wifiManager.addParameter(&portParam);
  wifiManager.addParameter(&topicRootParam);

  wifiManager.setSaveConfigCallback(saveConfigCallback);

  wifiManager.autoConnect("ESP-IR-sender");

  strcpy(mqttConfig.server, serverParam.getValue());
  strcpy(mqttConfig.topicRoot, topicRootParam.getValue());
  strcpy(mqttConfig.port, portParam.getValue());

  Serial << endl
         << "Connected to WiFi: " << WiFi.SSID() << endl
         << "IP address: " << WiFi.localIP() << endl;
}


void connectMQTT(PubSubClient &mqttClient, MqttConfiguration &mqttConfig, Client &client, Stream &stream, MQTT_CALLBACK_SIGNATURE) {
  mqttClient.setServer(mqttConfig.server, (uint16_t)atoi(mqttConfig.port));
  mqttClient.setClient(client);
  mqttClient.setStream(stream);
  mqttClient.setCallback(callback);

  // Loop until we're connected
  while(!mqttClient.connected()) {
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);

    Serial << "Connecting to " << mqttConfig.server << ":" << mqttConfig.port << " as " << clientId << endl;

    if(mqttClient.connect(clientId.c_str())) {
      Serial << "Connected. Subscribing to " << mqttConfig.topicRoot << "/prontohex and " << mqttConfig.topicRoot << "/reset" <<  endl;
      String s(mqttConfig.topicRoot);
      mqttClient.subscribe((s + "/prontohex").c_str());
      mqttClient.subscribe((s + "/reset").c_str());
    } else {
      Serial << "MQTT connection failed, rc=" << mqttClient.state() << " trying again in 5 seconds" << endl;
      delay(5000);
    }
  }
}
