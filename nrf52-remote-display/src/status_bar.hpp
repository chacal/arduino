#pragma once

#include "display.hpp"

class status_bar {
public:
  explicit status_bar(display &d): d(d) {};

  void update_rssi(int8_t rssi);

private:
  display &d;
};
