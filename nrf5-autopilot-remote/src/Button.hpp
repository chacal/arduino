#pragma once

#include <app_button.h>

class Button {
public:
  explicit Button(uint8_t pin) : pin{pin} {};

  void on_action(uint8_t button_action) {
    if (button_action == APP_BUTTON_PUSH) {
      pressed_time_ticks = app_timer_cnt_get();
    } else {
      uint32_t pressed_ms = 1000 * app_timer_cnt_diff_compute(app_timer_cnt_get(), pressed_time_ticks) / 32768;
      NRF_LOG_INFO("Pressed %d for %dms", pin, pressed_ms)
    }
  }

private:
  uint8_t  pin;
  uint32_t pressed_time_ticks = 0;
};
