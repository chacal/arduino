#include <hfsm/machine_single.hpp>
#include <nrf_log_ctrl.h>
#include <nrf_log.h>
#include <nrf_log_default_backends.h>
#include <app_scheduler.h>
#include <nrf_power.h>
#include "thread.hpp"
#include "state_machine.hpp"

// Set REG0 (high voltage DC/DC regulator) to output 3V. This becomes board's VDD and is supplied to external peripherals & GPIO.
volatile uint32_t m_uicr_regout0 __attribute__ ((section(".uicr_regout0"))) = UICR_REGOUT0_VOUT_3V0;

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

int main(int argc, char *argv[]) {
  APP_ERROR_CHECK(NRF_LOG_INIT(nullptr));
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  initialize_power_supply();
  APP_ERROR_CHECK(app_timer_init());  // Needs to be initialized before FSM as some states use app_timer in their constructors

  NRF_LOG_INFO("Starting Network Display "
                   APP_VERSION
                   "..")

  state_machine fsm;

  while (true) {
    if (fsm.update()) {
      app_sched_execute();
      thread::run();
    }
  }
}
