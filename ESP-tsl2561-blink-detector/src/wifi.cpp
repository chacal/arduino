#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "wifi_config.hpp"
#include "config.hpp"
#include "wifi.hpp"

static WiFiUDP         Udp;

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

void sendPacket(const String &msg, const String &dst_host) {
  Udp.beginPacket(dst_host.c_str(), DST_PORT);
  Udp.println(msg);
  Udp.endPacket();
}
