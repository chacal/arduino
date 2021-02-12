#include <nrfx_gpiote.h>

#include <utility>
#include <nrf_log.h>

#include "gpio_switch.hpp"


gpio_switch::gpio_switch(const uint8_t pin_number, std::string json_field) : pin_number{pin_number}, json_field(std::move(json_field)) {
  if (!nrfx_gpiote_is_init()) {
    APP_ERROR_CHECK(nrfx_gpiote_init());
  }
  nrf_gpio_cfg_output(pin_number);
}

void gpio_switch::turn_on() {
  nrf_gpio_pin_set(pin_number);
}

void gpio_switch::turn_off() {
  nrf_gpio_pin_clear(pin_number);
}

void gpio_switch::set_state_from_json(const JsonDocument &doc) {
  if (doc.containsKey(json_field)) {
    if (doc[json_field].is<bool>()) {
      bool state = doc[json_field];
      if (state) {
        turn_on();
      } else {
        turn_off();
      }
    } else {
      NRF_LOG_ERROR("%s must be a boolean!", json_field.c_str());
    }
  }
}

