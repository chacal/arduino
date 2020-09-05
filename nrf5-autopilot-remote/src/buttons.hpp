#pragma once

#define LONG_PRESS_TIME_MS 1000

enum button_id {
  BTN_ENABLE  = 1,
  BTN_DISABLE = 2,
  BTN_PLUS10  = 3,
  BTN_PLUS1   = 4,
  BTN_MINUS1  = 5,
  BTN_MINUS10 = 6
};

typedef void (*button_cb_t)(button_id btn_id, bool is_long_press);

void buttons_init(button_cb_t push_cb, button_cb_t release_cb);