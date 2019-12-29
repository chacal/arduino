#include <StreamPrint.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

#include "config.hpp"
#include "wifi.hpp"
#include "mqtt.hpp"
#include "util.hpp"

#define UART_TX_PIN_AFTER_SWAP   15
#define UART_RX_PIN_AFTER_SWAP   13
#define UART_RX_BUF_SIZE        200

#define RESET_PIN                13   // Pull this to GND during bootup to reset all settings (Wifi & configuration)

WiFiManager  wifiManager;
WiFiClient   wifiClient;
PubSubClient mqttClient;
char         buffer[UART_RX_BUF_SIZE];


// Setup & loop

void setup() {
  Serial.begin(234000);
  Serial.println("Starting ESP-BT-MQTT gateway..");
  blinkLed(1);

  resetAndRebootIfPinLow(RESET_PIN, wifiManager);

  loadConfigFromFile();
  randomSeed(micros());

  connectWiFi(wifiManager);
  blinkLed(3);
  delay(1000);

  printConfig();
  MDNS.begin(config.hostname);
  ArduinoOTA.begin();
  connectMQTT(mqttClient, wifiClient);

  Serial.pins(UART_TX_PIN_AFTER_SWAP, UART_RX_PIN_AFTER_SWAP);
  blinkLed(8);
}

void loop() {
  if (!mqttClient.connected()) {
    connectMQTT(mqttClient, wifiClient);
  }

  if (readline(Serial.read(), buffer, sizeof(buffer)) > 0) {
    publishToMqtt(mqttClient, buffer);
  }

  mqttClient.loop();
  MDNS.update();
  ArduinoOTA.handle();
}
