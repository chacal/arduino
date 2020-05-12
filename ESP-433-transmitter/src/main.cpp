#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>

#include "wifi.hpp"
#include "web_server.hpp"
#include "transmitter.hpp"

#define HOSTNAME   "esp-433-gateway"

void blinkLed() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(2);
  digitalWrite(LED_BUILTIN, HIGH);
}


void setup() {
  Serial.begin(115200);
  Serial.println("\nStarting ESP 433 transmitter");

  auto hostname = String(HOSTNAME);
  hostname.toLowerCase();

  connectWifi(hostname);

  MDNS.begin(hostname);
  ArduinoOTA.begin();

  tx_init();
  web_server_init();

  pinMode(LED_BUILTIN, OUTPUT);
  // Indicate that Wifi connection is established
  for (int i = 0; i < 8; ++i) {
    blinkLed();
    delay(100);
  }
}

void loop() {
  ArduinoOTA.handle();
  tx_process();
}