#include <stdint.h>
#include <nrf_sdm.h>
#include <nrf_sdh.h>
#include <nrf_log_default_backends.h>
#include <ble_dfu.h>
#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "sdk_config.h"
#include "ble_dfu_trigger_service.h"
#include "environmental_sensor.h"
#include "pir_sensor.h"

#define DEVICE_NAME                      "P300"


static void on_dfu_triggered() {
  NRF_LOG_INFO("Triggering DFU!")
  APP_ERROR_CHECK(sd_power_gpregret_clr(0, 0xffffffff));
  APP_ERROR_CHECK(sd_power_gpregret_set(0, BOOTLOADER_DFU_START));
  NVIC_SystemReset();
}

static void power_manage(void) {
  // Always clear FPU IRQs to allow CPU to sleep. See: https://devzone.nordicsemi.com/question/87838/high-power-consumption-when-using-fpu/
  __set_FPSCR(__get_FPSCR() & ~(0x0000009F));
  (void) __get_FPSCR();
  NVIC_ClearPendingIRQ(FPU_IRQn);
  uint32_t err_code = sd_app_evt_wait();
  APP_ERROR_CHECK(err_code);
}


int main(void) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  APP_ERROR_CHECK(app_timer_init());

  ble_dfu_trigger_service_init(DEVICE_NAME, on_dfu_triggered);

  //environmental_sensor_start();
  pir_sensor_start();

  for(;;) {
    power_manage();
  }
}
