#pragma once
#include <WiFiManager.h>

int readline(int readch, char *buffer, int buf_len);

void blinkLed(uint8_t times = 1);
void resetAndRebootIfPinLow(uint8_t pin, WiFiManager &wifiManager);
String protocolFromMqttUrl(const String &url);
String addressFromMqttUrl(const String &url);
void setClock();