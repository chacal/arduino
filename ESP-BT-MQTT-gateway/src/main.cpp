#include <ESP-MQTT-utils.h>
#include <StringUtils.h>

#define UART_TX_PIN_AFTER_SWAP   15
#define UART_RX_PIN_AFTER_SWAP   13
#define UART_RX_BUF_SIZE        200

int readline(int readch, char *buffer, int buf_len);
void publishToMqtt(char *str);
void connectWiFi();
void connectMQTT();
void resetConfigAndReboot();

MQTTConfigSaver   configSaver;
MqttConfiguration mqttConfig("mqtt-home.chacal.fi", "/test/bt-sensor-gw/1");
WiFiManager       wifiManager;
WiFiClient        wifiClient;
bool shouldSaveMQTTConfig = false;
PubSubClient      mqttClient;


// Setup & loop

void setup() {
  Serial.begin(234000);
  Serial.println("Starting ESP-BT-MQTT gateway..");

  randomSeed(micros());

  configSaver.loadConfiguration(mqttConfig);
  connectWiFi();
  if(shouldSaveMQTTConfig) {
    configSaver.saveConfiguration(mqttConfig);
  }
  connectMQTT();

  Serial.pins(UART_TX_PIN_AFTER_SWAP, UART_RX_PIN_AFTER_SWAP);
}

void loop() {
  if(!mqttClient.connected()) {
    connectMQTT();
  }

  static char buffer[UART_RX_BUF_SIZE];
  if(readline(Serial.read(), buffer, sizeof(buffer)) > 0) {
    publishToMqtt(buffer);
  }

  mqttClient.loop();
}



// MQTT message handling

void publishToMqtt(char *str) {
  String s(mqttConfig.topicRoot);
  mqttClient.publish((s + "/value").c_str(), str, false);
}

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

int readline(int readch, char *buffer, int buf_len) {
  static int pos = 0;
  int        rpos;

  if(readch > 0) {
    switch(readch) {
      case '\r': // Ignore CR new-lines
        break;
      case '\n': // Return on new-lines
        rpos = pos;
        pos  = 0;  // Reset position index ready for next time
        return rpos;
      default:
        if(pos < buf_len - 1) {
          buffer[pos++] = readch;
          buffer[pos]   = 0;
        }
    }
  }
  // No end of line has been found, so return -1.
  return -1;
}
