#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "wifi_config.hpp"
#include "config.hpp"
#include "wifi.hpp"

static WiFiUDP Udp;

void connectWifi(const String &hostname) {
  Serial.printf("Connecting to %s ", WIFI_SSID);
  WiFi.hostname(hostname);
  WiFi.begin(WIFI_SSID, WIFI_PSK);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void sendPacket(const String &msg, const String &dst_host, uint16_t port) {
  Udp.beginPacket(dst_host.c_str(), port);
  Udp.println(msg);
  Udp.endPacket();
}

void sendPacket(udp_body_writer body_writer, const String &dst_host, uint16_t port) {
  Udp.beginPacket(dst_host.c_str(), port);
  body_writer(Udp);
  Udp.endPacket();
}