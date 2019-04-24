#include <nrf_log_ctrl.h>
#include <nrf_log.h>
#include <nrf_log_default_backends.h>
#include <epd2in9/epd2in9.h>

extern "C" {
#include <nrf_pwr_mgmt.h>
}

static void log_init() {
  ret_code_t err_code = NRF_LOG_INIT(nullptr);
  APP_ERROR_CHECK(err_code);

  NRF_LOG_DEFAULT_BACKENDS_INIT();
}

static void power_management_init() {
  ret_code_t err_code;
  err_code = nrf_pwr_mgmt_init();
  APP_ERROR_CHECK(err_code);
}


int main(int argc, char *argv[]) {
  log_init();
  power_management_init();

  NRF_LOG_INFO("E-Display test started");

  auto epd = Epd();

  for (;;) {
    nrf_pwr_mgmt_run();
  }
}
