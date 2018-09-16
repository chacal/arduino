#include <nrf_log_default_backends.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <hfsm/machine_single.hpp>
#include "power_manager.hpp"
#include "display.hpp"

int main() {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  power_manager::init();

  display d;
  d.on();
  d.draw_str(point{0, 12}, font_size(10), "Hello world!");
  d.draw_line(point{0, 13}, point{128, 13});
  d.render();

  NRF_LOG_INFO("Hello world!")

  for (;;) {
    power_manager::manage();
  }
}
