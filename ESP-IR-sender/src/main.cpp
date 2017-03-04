#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <LoopbackStream.h>
#include <PubSubClient.h>
#include <StreamPrint.h>
#include <WiFiManager.h>
#include "prontoConverter.h"
#include "ConfigSaver.h"
#include "utils.h"


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
  connectWiFi(wifiManager, mqttConfig, []() { shouldSaveMQTTConfig = true; });
}

void connectMQTT() {
  connectMQTT(mqttClient, mqttConfig, wifiClient, mqttInputBuffer, mqttCallback);
}
