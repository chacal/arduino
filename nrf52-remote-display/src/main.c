#include <nrf_log_default_backends.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "power_manager.h"

int main(void) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  power_manager_init();

  NRF_LOG_INFO("Hello world!")

  for(;;) {
    power_manage();
  }
}
