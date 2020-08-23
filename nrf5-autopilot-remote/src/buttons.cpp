#include <nrf_log.h>
#include <app_timer.h>
#include <app_button.h>
#include <unordered_map>
#include "Button.hpp"

#define BUTTON_PIN1          13
#define BUTTON_PIN2          14
#define BUTTON_PIN3          15
#define BUTTON_DEBOUNCE_MS   APP_TIMER_TICKS(10)

std::unordered_map<uint8_t, Button> buttons = {
  {BUTTON_PIN1, Button(BUTTON_PIN1)},
  {BUTTON_PIN2, Button(BUTTON_PIN2)},
  {BUTTON_PIN3, Button(BUTTON_PIN3)},
};


void on_button(uint8_t pin_no, uint8_t button_action) {
  buttons.at(pin_no).on_action(button_action);
}

void buttons_init() {
  APP_ERROR_CHECK(app_timer_init());
  static app_button_cfg_t buttons[] =
                            {
                              {BUTTON_PIN1, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, on_button},
                              {BUTTON_PIN2, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, on_button},
                              {BUTTON_PIN3, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, on_button},
                            };

  APP_ERROR_CHECK(app_button_init(buttons, ARRAY_SIZE(buttons), BUTTON_DEBOUNCE_MS));
  APP_ERROR_CHECK(app_button_enable());
}