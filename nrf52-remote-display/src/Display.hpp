#pragma once

#include <stdint.h>

extern "C" {
#include <u8g2.h>
}

class Display {
private:
  Display();
  u8g2_t u8g2;

public:
  static Display &getInstance();
  void on();
  void off();
  void drawStr(uint8_t x, uint8_t y, uint8_t fontSize, char *str);
  void drawLine(uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY);
  void render();
  void clear();
  uint8_t centeredX(const char *str, uint8_t fontSize);
};
