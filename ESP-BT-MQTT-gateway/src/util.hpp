#pragma once

int readline(int readch, char *buffer, int buf_len);

void blinkLed(uint8_t times = 1);
void resetAndRebootIfPinLow(uint8_t pin, WiFiManager &wifiManager);