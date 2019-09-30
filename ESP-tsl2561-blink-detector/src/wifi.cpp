#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "wifi_config.hpp"
#include "wifi.hpp"

static WiFiUDP         Udp;
static udp_packet_cb_t m_udp_packet_cb;


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

void sendPacket(uint32_t data, const String &dst_host) {
  Udp.beginPacket(dst_host.c_str(), DST_PORT);
  Udp.println(data);
  Udp.endPacket();
}

void udp_server_init(uint16_t port, udp_packet_cb_t on_udp_packet) {
  m_udp_packet_cb = on_udp_packet;
  Udp.begin(port);
  Serial.printf("Listening on UDP port %u\n", port);
}

void udp_server_receive() {
  static char incoming_packet[255];

  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int len = Udp.read(incoming_packet, sizeof(incoming_packet));
    if (len > 0) {
      incoming_packet[len] = 0;
      m_udp_packet_cb(incoming_packet);
    }
  }
}