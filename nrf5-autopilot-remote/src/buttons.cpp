#include <nrf_log.h>
#include <app_timer.h>
#include <app_button.h>

#define BUTTON_PIN  13
#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(20)

uint32_t millis() {
  return (app_timer_cnt_get() / 32.768);
}

void on_button(uint8_t pin_no, uint8_t button_action) {
  NRF_LOG_INFO("Button pressed! %d %d %d", pin_no, button_action, millis())
}

void buttons_init() {
  APP_ERROR_CHECK(app_timer_init());
  static app_button_cfg_t buttons[] =
                            {
                              {BUTTON_PIN, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, on_button},
                              {14,         APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, on_button},
                              {15,         APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, on_button},
                            };

  APP_ERROR_CHECK(app_button_init(buttons, ARRAY_SIZE(buttons), BUTTON_DETECTION_DELAY));
  APP_ERROR_CHECK(app_button_enable());
}