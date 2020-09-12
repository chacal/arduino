#include <nrf_log.h>
#include <app_timer.h>
#include <app_button.h>
#include <unordered_map>
#include "Button.hpp"

#define BUTTON_ENABLE_PIN       2
#define BUTTON_DISABLE_PIN      3
#define BUTTON_MINUS10_PIN     11
#define BUTTON_MINUS1_PIN      12
#define BUTTON_PLUS1_PIN       13
#define BUTTON_PLUS10_PIN      14
#define BUTTON_DEBOUNCE_MS   APP_TIMER_TICKS(10)

std::unordered_map<uint8_t, Button> buttons;


void on_button(uint8_t pin_no, uint8_t button_action) {
  buttons.at(pin_no).on_action(button_action);
}

void buttons_init(button_cb_t push_cb, button_cb_t release_cb) {
  buttons                           = {
    {BUTTON_ENABLE_PIN,  Button(BTN_ENABLE, push_cb, release_cb)},
    {BUTTON_DISABLE_PIN, Button(BTN_DISABLE, push_cb, release_cb)},
    {BUTTON_MINUS10_PIN, Button(BTN_MINUS10, push_cb, release_cb)},
    {BUTTON_MINUS1_PIN, Button(BTN_MINUS1, push_cb, release_cb)},
    {BUTTON_PLUS1_PIN, Button(BTN_PLUS1, push_cb, release_cb)},
    {BUTTON_PLUS10_PIN, Button(BTN_PLUS10, push_cb, release_cb)},
  };

  APP_ERROR_CHECK(app_timer_init());
  static app_button_cfg_t buttons[] =
                            {
                              {BUTTON_ENABLE_PIN,  APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, on_button},
                              {BUTTON_DISABLE_PIN, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, on_button},
                              {BUTTON_MINUS10_PIN, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, on_button},
                              {BUTTON_MINUS1_PIN, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, on_button},
                              {BUTTON_PLUS1_PIN, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, on_button},
                              {BUTTON_PLUS10_PIN, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, on_button},
                            };

  APP_ERROR_CHECK(app_button_init(buttons, ARRAY_SIZE(buttons), BUTTON_DEBOUNCE_MS));
  APP_ERROR_CHECK(app_button_enable());
}