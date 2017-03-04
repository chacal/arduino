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


ConfigSaver configSaver;
WiFiManager wifiManager;
WiFiClient wifiClient;
MqttConfiguration mqttConfig("mqtt-home.chacal.online", "/test/irsender/1");
bool shouldSaveMQTTConfig = false;
LoopbackStream mqttInputBuffer(1024);
PubSubClient mqttClient;
IRsend irsend(3);


void setup(void) {
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

bool endsWith(const char *string, const char *suffix) {
  if(!string || !suffix)
    return false;

  string = strrchr(string, suffix[0]);

  if(string != NULL)
    return strcmp(string, suffix) == 0;

  return false;
}

void resetConfigAndReboot() {
  Serial << "Resetting configuration." << endl;
  configSaver.removeSavedConfig();
  wifiManager.resetSettings();
  Serial << "Rebooting.." << endl;
  ESP.restart();
}

void handleProntoHexMessage(const char *msg) {
  RawSampleData sampleData;
  bool ret = convertProntoToRaw(msg, &sampleData);

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

void mqttCallback(char *topic, uint8_t *payload, unsigned int length) {
  Serial << "Got MQTT message: " << topic << endl;

  if(endsWith(topic, "/reset")) {
    resetConfigAndReboot();
  } else if(endsWith(topic, "/prontohex")) {
    handleProntoHexMessage(mqttInputBuffer.readString().c_str());
  }

  return;
}

void connectWiFi() {
  Serial << "Connecting to WiFi.." << endl;

  WiFiManagerParameter serverParam("server", "MQTT server", mqttConfig.server, 100);
  WiFiManagerParameter portParam("port", "MQTT port", mqttConfig.port, 6);
  WiFiManagerParameter topicRootParam("topic_root", "MQTT topic root", mqttConfig.topicRoot, 100);

  wifiManager.addParameter(&serverParam);
  wifiManager.addParameter(&portParam);
  wifiManager.addParameter(&topicRootParam);

  wifiManager.setSaveConfigCallback([]() { shouldSaveMQTTConfig = true; });

  wifiManager.autoConnect("ESP-IR-sender");

  strcpy(mqttConfig.server, serverParam.getValue());
  strcpy(mqttConfig.topicRoot, topicRootParam.getValue());
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
