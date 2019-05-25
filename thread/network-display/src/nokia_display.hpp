#pragma once

#include "nrf_u8g2_adapter.hpp"
#include "display.hpp"

class nokia_display : public display {
public:
  nokia_display();

  void on() override;

  void clear() override;

  void render() override;

private:
  nrf_u8g2_adapter adapter;
};
