#pragma once

void connectWifi();


typedef void (*udp_packet_cb_t)(const String &);
void sendPacket(const String &msg, const String &dst_host, uint16_t port);

void udp_server_init(uint16_t port, udp_packet_cb_t on_udp_packet);

void udp_server_receive();