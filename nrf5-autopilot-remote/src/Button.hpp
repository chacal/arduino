#pragma once

#include <app_button.h>

#include "buttons.hpp"

class Button {
public:
  explicit Button(button_id id, button_cb_t push_cb, button_cb_t release_cb) : id{id}, push_cb{push_cb}, release_cb{release_cb} {};

  void on_action(uint8_t button_action) {
    if (button_action == APP_BUTTON_PUSH) {
      pressed_time_ticks = app_timer_cnt_get();
      push_cb(id, false);
    } else {
      uint32_t pressed_ms = 1000 * app_timer_cnt_diff_compute(app_timer_cnt_get(), pressed_time_ticks) / 32768;
      release_cb(id, pressed_ms > LONG_PRESS_TIME_MS);
    }
  }

private:
  button_id   id;
  button_cb_t push_cb;
  button_cb_t release_cb;
  uint32_t    pressed_time_ticks = 0;
};
