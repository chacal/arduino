#include <nrf_log_ctrl.h>
#include <nrf_log.h>
#include <nrf_log_default_backends.h>

int main(int argc, char *argv[]) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();


  NRF_LOG_INFO("Starting Network Display " APP_VERSION "..")

  while (true) {
  }
}
