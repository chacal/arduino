#include "power_manager.hpp"

#include <app_error.h>
#include <nrf_log.h>
#include <nrf_power.h>

extern "C" {
#include <nrf_pwr_mgmt.h>
}

// Set REG0 (high voltage DC/DC regulator) to output 3V. This becomes board's VDD and is supplied to external peripherals & GPIO.
volatile uint32_t m_uicr_regout0 __attribute__ ((section(".uicr_regout0"))) = UICR_REGOUT0_VOUT_3V0;

namespace power_manager {

  uint32_t get_regout0() {
    return NRF_UICR->REGOUT0 & UICR_REGOUT0_VOUT_Msk;
  }

  void initialize_power_supply() {
    nrf_power_dcdcen_set(true);
    nrf_power_dcdcen_vddh_set(true);
    NRF_LOG_INFO("REGOUT0: %d", get_regout0())
    NRF_LOG_INFO("Main reg status: %d", nrf_power_mainregstatus_get())
    NRF_LOG_INFO("REG0 DCDC enabled: %d", nrf_power_dcdcen_vddh_get())
    NRF_LOG_INFO("REG1 DCDC enabled: %d", nrf_power_dcdcen_get())
  }

  void init() {
    initialize_power_supply();
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



