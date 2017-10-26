#include <nrf.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <app_error.h>

int main(void) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_INFO("Starting..\n");

  for(;;) {
  }
}
