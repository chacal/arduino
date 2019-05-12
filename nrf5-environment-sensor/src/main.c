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
#include "current_sensor.h"
#include "ntc_thermometer_sensor.h"
#include "tank_sensor.h"
#include "dcdc_selector.h"

#define SENSOR_TYPE_BME280       1
#define SENSOR_TYPE_PIR          2
#define SENSOR_TYPE_INA226       3
#define SENSOR_TYPE_NTC          4
#define SENSOR_TYPE_TANK_LEVEL   5


#define DEVICE_NAME         S217
#define DCDC_MODE           DCDC_MODE_BY_DEVICE_NAME   // Either DCDC_MODE_DISABLED, DCDC_MODE_ENABLED or DCDC_MODE_BY_DEVICE_NAME
#define SENSOR_TYPE         SENSOR_TYPE_BME280         // Remember also to check the PIR sensor pins in pir_sensor.c!


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

static void setup_dcdc() {
#if DCDC_MODE == DCDC_MODE_DISABLED
  dcdc_init(NRF_POWER_DCDC_DISABLE);
#elif DCDC_MODE == DCDC_MODE_ENABLED
  dcdc_init(NRF_POWER_DCDC_ENABLE);
#else
  dcdc_init(DCDC_MODE_FOR_NAME(DEVICE_NAME));
#endif
}

static void start_selected_sensor() {
#if SENSOR_TYPE == SENSOR_TYPE_BME280
  environmental_sensor_start();
#elif SENSOR_TYPE == SENSOR_TYPE_PIR
  pir_sensor_start();
#elif SENSOR_TYPE == SENSOR_TYPE_INA226
  current_sensor_start();
#elif SENSOR_TYPE == SENSOR_TYPE_NTC
  ntc_thermometer_sensor_start();
#elif SENSOR_TYPE == SENSOR_TYPE_TANK_LEVEL
  tank_sensor_start();
#else
#error Unknown sensor type!
#endif
}

int main(void) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  APP_ERROR_CHECK(app_timer_init());

  ble_dfu_trigger_service_init(STR(DEVICE_NAME), on_dfu_triggered);
  setup_dcdc();
  start_selected_sensor();

  for (;;) {
    power_manage();
  }
}
