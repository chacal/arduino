#include <nrf_log_default_backends.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include "power_manager.hpp"
#include "Display.hpp"

int main() {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  power_manager::init();

  Display d = Display::getInstance();
  d.on();
  d.drawStr(0, 12, 10, (char*)"Hello world!");
  d.drawLine(0, 13, 128, 13);
  d.render();

  NRF_LOG_INFO("Hello world!")

  for(;;) {
    power_manager::manage();
  }
}
