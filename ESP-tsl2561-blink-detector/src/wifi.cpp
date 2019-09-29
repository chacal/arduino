#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "wifi_config.hpp"

static WiFiUDP Udp;

void connectWifi() {
  Serial.printf("Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PSK);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void sendPacket(uint32_t data) {
  Udp.beginPacket(DST_HOST, DST_PORT);
  Udp.println(data);
  Udp.endPacket();
}