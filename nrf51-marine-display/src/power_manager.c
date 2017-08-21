#include "power_manager.h"

#include <app_error.h>
#include <nrf_soc.h>

void power_manage() {
  uint32_t err_code = sd_app_evt_wait();
  APP_ERROR_CHECK(err_code);
}
