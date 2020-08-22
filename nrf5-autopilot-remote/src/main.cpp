#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


int main(void) {
  APP_ERROR_CHECK(NRF_LOG_INIT(nullptr));
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  NRF_LOG_INFO("nRF5 autopilot remote starting..")

  for (;;) {
  }
}