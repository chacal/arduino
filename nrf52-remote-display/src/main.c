#include <nrf_log_default_backends.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "power_manager.h"
#include "display.h"

int main(void) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  power_manager_init();
  display_init();

  display_on();
  display_draw_str(0, 12, 10, "Hello world!");
  display_draw_line(0, 13, 128, 13);
  display_render();

  NRF_LOG_INFO("Hello world!")

  for(;;) {
    power_manage();
  }
}
