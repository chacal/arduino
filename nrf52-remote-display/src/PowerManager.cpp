#include "PowerManager.hpp"

#include <app_error.h>
#include <nrf_log.h>
extern "C" {
#include <nrf_pwr_mgmt.h>
}

PowerManager::PowerManager() {
  uint32_t error = nrf_pwr_mgmt_init();
  APP_ERROR_CHECK(error);
}

PowerManager &PowerManager::getInstance() {
  static PowerManager theInstance;
  return theInstance;
}

void PowerManager::manage() {
  nrf_pwr_mgmt_run();
}

void PowerManager::shutdown() {
  NRF_LOG_INFO("Shutdown\n");
  nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
}
