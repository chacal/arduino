#pragma once

#include "display.hpp"

#define STATUS_BAR_WIDTH  128
#define STATUS_BAR_HEIGHT   4

class status_bar {
public:
  explicit status_bar(display &d): d(d) {};

  void update_rssi(int8_t rssi);

private:
  display &d;
};
