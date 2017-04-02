#include <RCSwitch.h>
#include <ESP-MQTT-utils.h>
#include <ArduinoJson.h>
#include <StringUtils.h>

#define MESSAGE_THROTTLE_MS 2000    // Don't publish MQTT messages more often than this


void connectWiFi();
void connectMQTT();
void handleSwitchMessage();
void resetConfigAndReboot();


RCSwitch mySwitch = RCSwitch();
MQTTConfigSaver configSaver;
MqttConfiguration mqttConfig("mqtt-home.chacal.fi", "/test/433gw/1");
WiFiManager wifiManager;
WiFiClient wifiClient;
bool shouldSaveMQTTConfig = false;
PubSubClient mqttClient;


// Setup & loop

void setup() {
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);

  randomSeed(micros());

  configSaver.loadConfiguration(mqttConfig);
  connectWiFi();
  if(shouldSaveMQTTConfig) {
    configSaver.saveConfiguration(mqttConfig);
  }
  connectMQTT();

  mySwitch.enableReceive(digitalPinToInterrupt(3));  // GPIO3 is UART0 RX
}

void loop() {
  if(!mqttClient.connected()) {
    connectMQTT();
  }

  if(mySwitch.available()) {
    handleSwitchMessage();
    mySwitch.resetAvailable();
  }

  mqttClient.loop();
}


// Radio message handling

void handleSwitchMessage() {
  static int previousValue;
  static unsigned long previousTime;

  if(previousTime > millis()) {
    previousTime = 0;  // Handle millis() overflow
  }

  int value = mySwitch.getReceivedValue();

  if(value == 0) {
    Serial << "Unknown encoding" << endl;
  } else if(value != previousValue || millis() - previousTime > MESSAGE_THROTTLE_MS) {
    Serial << "Received message " << mySwitch.getReceivedValue() << endl;

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

    previousValue = value;
    previousTime = millis();
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
  Serial << "Subscribing to " << String(mqttConfig.topicRoot) + "/reset" << endl;
  mqttClient.subscribe((String(mqttConfig.topicRoot) + "/reset").c_str());
}

void resetConfigAndReboot() {
  Serial << "Resetting configuration." << endl;
  configSaver.removeSavedConfig();
  wifiManager.resetSettings();
  delay(1000);
  Serial << "Rebooting.." << endl;
  delay(2000);
  ESP.restart();
}
