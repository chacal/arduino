#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <LoopbackStream.h>
#include <PubSubClient.h>
#include <StreamPrint.h>
#include <WiFiManager.h>
#include "prontoConverter.h"
#include "ConfigSaver.h"


void connectWiFi();
void connectMQTT();


MqttConfiguration mqttConfig("mqtt-home.chacal.online", "/test/irsender/1/prontohex");
bool shouldSaveMQTTConfig = false;
LoopbackStream mqttInputBuffer(1024);
WiFiClient wifiClient;
PubSubClient mqttClient;
IRsend irsend(3);


void setup(void) {
  ConfigSaver configSaver;

  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  randomSeed(micros());
  irsend.begin();

  configSaver.loadConfiguration(mqttConfig);
  connectWiFi();
  if(shouldSaveMQTTConfig) {
    configSaver.saveConfiguration(mqttConfig);
  }
  connectMQTT();
}

void loop(void) {
  if(!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();
}

void mqttCallback(char *topic, uint8_t *payload, unsigned int length) {
  RawSampleData sampleData;
  bool ret = convertProntoToRaw(mqttInputBuffer.readString().c_str(), &sampleData);

  if(ret) {
    Serial << "Got valid pronto data. Freq: " << (int) sampleData.freq << " Sample count: "
           << (int) sampleData.sampleCount << endl;
    for(int i = 0; i < 3; ++i) {
      irsend.sendRaw(sampleData.samples, sampleData.sampleCount, sampleData.freq);
      delay(20);
    }

  } else {
    Serial << "Couldn't convert message to raw samples!" << endl;
  }
}

void connectWiFi() {
  Serial << "Connecting to WiFi.." << endl;

  WiFiManager wifiManager;
  WiFiManagerParameter serverParam("server", "MQTT server", mqttConfig.server, 100);
  WiFiManagerParameter portParam("port", "MQTT port", mqttConfig.port, 6);
  WiFiManagerParameter topicParam("topic", "MQTT topic", mqttConfig.topic, 100);

  wifiManager.addParameter(&serverParam);
  wifiManager.addParameter(&portParam);
  wifiManager.addParameter(&topicParam);

  wifiManager.setSaveConfigCallback([]() { shouldSaveMQTTConfig = true; });

  wifiManager.autoConnect("ESP-IR-sender");

  strcpy(mqttConfig.server, serverParam.getValue());
  strcpy(mqttConfig.topic, topicParam.getValue());
  strcpy(mqttConfig.port, portParam.getValue());

  Serial << endl
         << "Connected to WiFi: " << WiFi.SSID() << endl
         << "IP address: " << WiFi.localIP() << endl;
}

void connectMQTT() {
  mqttClient.setServer(mqttConfig.server, (uint16_t)atoi(mqttConfig.port));
  mqttClient.setClient(wifiClient);
  mqttClient.setStream(mqttInputBuffer);
  mqttClient.setCallback(mqttCallback);

  // Loop until we're connected
  while(!mqttClient.connected()) {
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);

    Serial << "Connecting to " << mqttConfig.server << ":" << mqttConfig.port << mqttConfig.topic << " as " << clientId << endl;

    if(mqttClient.connect(clientId.c_str())) {
      Serial << "connected" << endl;
      mqttClient.subscribe(mqttConfig.topic);
    } else {
      Serial << "failed, rc=" << mqttClient.state() << " trying again in 5 seconds" << endl;
      delay(5000);
    }
  }
}
