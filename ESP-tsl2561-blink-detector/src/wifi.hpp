#pragma once

#include <Udp.h>

void connectWifi();

void sendPacket(const String &msg, const String &dst_host, uint16_t port);

typedef void (*udp_body_writer)(UDP &);

void sendPacket(udp_body_writer body_writer, const String &dst_host, uint16_t port);
