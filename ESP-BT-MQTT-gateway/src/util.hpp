#pragma once

int readline(int readch, char *buffer, int buf_len);

void resetConfigAndReboot(const WiFiManager &);
void blinkLed(uint8_t times = 1);