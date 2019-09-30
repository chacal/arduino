#pragma once

void connectWifi();

void sendPacket(uint32_t data, const String &dst_host);

typedef void (*udp_packet_cb_t)(const String &);

void udp_server_init(uint16_t port, udp_packet_cb_t on_udp_packet);

void udp_server_receive();