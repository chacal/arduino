#include <RCSwitch.h>
#include <ESP-MQTT-utils.h>
#include <ArduinoJson.h>
#include <StringUtils.h>


void connectWiFi();
void connectMQTT();
void handleSwitchMessage();
void resetConfigAndReboot();


RCSwitch mySwitch = RCSwitch();
MQTTConfigSaver configSaver;
MqttConfiguration mqttConfig("mqtt-home.chacal.online", "/test/433gw/1");
WiFiManager wifiManager;
WiFiClient wifiClient;
bool shouldSaveMQTTConfig = false;
PubSubClient mqttClient;


void setup() {
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);

  randomSeed(micros());

  configSaver.loadConfiguration(mqttConfig);
  connectWiFi();
  if(shouldSaveMQTTConfig) {
    configSaver.saveConfiguration(mqttConfig);
  }
  connectMQTT();

  mySwitch.enableReceive(digitalPinToInterrupt(3));
}

void loop() {
  if(mySwitch.available()) {
    handleSwitchMessage();
    mySwitch.resetAvailable();
  }
}

void handleSwitchMessage() {
  int value = mySwitch.getReceivedValue();

  if(value == 0) {
    Serial << "Unknown encoding" << endl;
  } else {
    const int BUF_SIZE = 200;
    StaticJsonBuffer<BUF_SIZE> jsonBuffer;

    JsonObject &root = jsonBuffer.createObject();
    root["value"] = mySwitch.getReceivedValue();
    root["bits"] = mySwitch.getReceivedBitlength();
    root["protocol"] = mySwitch.getReceivedProtocol();

    char payload[BUF_SIZE];
    root.printTo(payload, BUF_SIZE);

    String s(mqttConfig.topicRoot);
    mqttClient.publish((s + "/value").c_str(), payload, true);
  }
}


// MQTT message handling

void mqttCallback(char *topic, uint8_t *payload, unsigned int length) {
  Serial << "Got MQTT message: " << topic << endl;
  if(endsWith(topic, "/reset")) {
    resetConfigAndReboot();
  }
}


// Helpers

void connectWiFi() {
  connectWiFi(wifiManager, mqttConfig, []() { shouldSaveMQTTConfig = true; });
}

void connectMQTT() {
  connectMQTT(mqttClient, mqttConfig, wifiClient, mqttCallback);
  mqttClient.subscribe((String(mqttConfig.topicRoot) + "/reset").c_str());
}

void resetConfigAndReboot() {
  Serial << "Resetting configuration." << endl;
  configSaver.removeSavedConfig();
  wifiManager.resetSettings();
  Serial << "Rebooting.." << endl;
  ESP.restart();
}
