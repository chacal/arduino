#include "power_manager.hpp"

#include <app_error.h>
#include <nrf_log.h>

extern "C" {
#include <nrf_pwr_mgmt.h>
}

namespace power_manager {
  void init() {
    uint32_t error = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(error);
  }

  void manage() {
    nrf_pwr_mgmt_run();
  }

  void shutdown() {
    NRF_LOG_INFO("Shutdown\n");
    nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
  }
}



