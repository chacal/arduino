#include <app_error.h>
#include <nrf_log.h>
#include <nrf_drv_twi.h>
#include "ina226.h"
#include <app_timer.h>
#include <app_util_platform.h>

#include "sdk_config.h"

#define PIN_SCL           2
#define PIN_SCA           3

#define INA226_ADDRESS                 0x76


typedef enum {
  BME280_REG_ID        = 0xD0,
  BME280_REG_CTRL_HUM  = 0xF2,
  BME280_REG_CTRL_MEAS = 0xF4,
  BME280_REG_CONFIG    = 0xF5,
  BME280_REG_PRESSURE  = 0xF7,
  BME280_REG_TEMP_DIG  = 0x88,
  BME280_REG_PRESS_DIG = 0x8E,
  BME280_REG_HUM_DIG_1 = 0xA1,
  BME280_REG_HUM_DIG_2 = 0xE1
}                              ina226_reg_t;


APP_TIMER_DEF(m_measurement_timer);
static nrf_drv_twi_t           m_twi = NRF_DRV_TWI_INSTANCE(0);
static ina226_measurement_cb_t m_measurement_cb;


static void read(uint8_t reg, uint8_t *buf, uint8_t len) {
  APP_ERROR_CHECK(nrf_drv_twi_tx(&m_twi, INA226_ADDRESS, &reg, 1, true));
  APP_ERROR_CHECK(nrf_drv_twi_rx(&m_twi, INA226_ADDRESS, buf, len));
}

static void write(uint8_t reg, uint8_t data) {
  uint8_t buf[2] = {reg, data};
  APP_ERROR_CHECK(nrf_drv_twi_tx(&m_twi, INA226_ADDRESS, buf, sizeof(buf), false));
}



static void twi_init(void) {
  const nrf_drv_twi_config_t twi_config = {
      .scl                = PIN_SCL,
      .sda                = PIN_SCA,
      .frequency          = NRF_TWI_FREQ_400K,
      .interrupt_priority = APP_IRQ_PRIORITY_LOW,
      .clear_bus_init     = false
  };

  APP_ERROR_CHECK(nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL));
  nrf_drv_twi_enable(&m_twi);
}


static void on_measurement_timer(void *ctx) {
  NRF_LOG_INFO("Measuring!")
}


void ina226_init(uint32_t measurement_interval_ms, ina226_measurement_cb_t callback) {
  m_measurement_cb          = callback;

  twi_init();

  app_timer_create(&m_measurement_timer, APP_TIMER_MODE_REPEATED, on_measurement_timer);
  app_timer_start(m_measurement_timer, APP_TIMER_TICKS(measurement_interval_ms), NULL);
}
