#include "power_manager.h"

#include <app_error.h>
#include <nrf_pwr_mgmt.h>
#include <app_timer.h>
#include <nrf_gpio.h>


void power_manager_init() {
  uint32_t error = nrf_pwr_mgmt_init(APP_TIMER_TICKS(1000, 0));
  APP_ERROR_CHECK(error);
}

void power_manage() {
  nrf_pwr_mgmt_run();
}

void power_manager_shutdown() {
  nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
}