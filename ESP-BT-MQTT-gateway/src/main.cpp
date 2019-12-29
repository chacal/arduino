#include <StreamPrint.h>

#include "config.hpp"
#include "wifi.hpp"
#include "mqtt.hpp"
#include "util.hpp"

#define UART_TX_PIN_AFTER_SWAP   15
#define UART_RX_PIN_AFTER_SWAP   13
#define UART_RX_BUF_SIZE        200


WiFiManager  wifiManager;
WiFiClient   wifiClient;
PubSubClient mqttClient;
char         buffer[UART_RX_BUF_SIZE];


// Setup & loop

void setup() {
  Serial.begin(234000);
  Serial.println("Starting ESP-BT-MQTT gateway..");
  loadConfigFromFile();

  randomSeed(micros());

  connectWiFi(wifiManager);
  printConfig();
  connectMQTT(mqttClient, wifiClient);

  Serial.pins(UART_TX_PIN_AFTER_SWAP, UART_RX_PIN_AFTER_SWAP);
}

void loop() {
  if (!mqttClient.connected()) {
    connectMQTT(mqttClient, wifiClient);
  }

  if (readline(Serial.read(), buffer, sizeof(buffer)) > 0) {
    publishToMqtt(mqttClient, buffer);
  }

  mqttClient.loop();
}
