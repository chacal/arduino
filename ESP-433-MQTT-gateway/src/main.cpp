#include <RCSwitch.h>
#include <ESP-MQTT-utils.h>
#include <LoopbackStream.h>


void connectWiFi();
void connectMQTT();


RCSwitch mySwitch = RCSwitch();
MQTTConfigSaver configSaver;
MqttConfiguration mqttConfig("mqtt-home.chacal.online", "/test/433gw/1");
WiFiManager wifiManager;
WiFiClient wifiClient;
bool shouldSaveMQTTConfig = false;
LoopbackStream mqttInputBuffer(1024);
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

    int value = mySwitch.getReceivedValue();

    if(value == 0) {
      Serial.print("Unknown encoding");
    } else {
      Serial.print("Received ");
      Serial.print(mySwitch.getReceivedValue());
      Serial.print(" / ");
      Serial.print(mySwitch.getReceivedBitlength());
      Serial.print("bit ");
      Serial.print("Protocol: ");
      Serial.println(mySwitch.getReceivedProtocol());
    }

    mySwitch.resetAvailable();
  }
}


// MQTT message handling

void mqttCallback(char *topic, uint8_t *payload, unsigned int length) {
  Serial << "Got MQTT message: " << topic << endl;
}


// Helpers

void connectWiFi() {
  connectWiFi(wifiManager, mqttConfig, []() { shouldSaveMQTTConfig = true; });
}

void connectMQTT() {
  connectMQTT(mqttClient, mqttConfig, wifiClient, mqttInputBuffer, mqttCallback);
}
