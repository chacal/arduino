#include <app_error.h>
#include <nrf_log.h>
#include <nrf_drv_twi.h>
#include "ina226.h"
#include <app_timer.h>
#include <app_util_platform.h>

#define PIN_SDA                        2
#define PIN_SCL                        3
#define INA226_ADDRESS              0x40
#define INA226_SHUNT_VOLTAGE_LSB_UV 2.5f
#define INA226_BUS_VOLTAGE_LSB_MV  1.25f


typedef enum {
  INA226_REG_CONFIG          = 0x00,
  INA226_REG_SHUNT_VOLTAGE   = 0x01,
  INA226_REG_BUS_VOLTAGE     = 0x02,
  INA226_REG_SHUNT_CURRENT   = 0x04,
  INA226_REG_CALIBRATION     = 0x05,
  INA226_REG_MANUFACTURER_ID = 0xFE,
  INA226_REG_DIE_ID          = 0xFF,
}                              ina226_reg_t;

typedef enum {
  INA226_CONF_SHUNT_AND_BUS_CONTINUOUS = 0x07,
  INA226_CONF_BUS_CONVERSION_8MS       = 0x7,
  INA226_CONF_SHUNT_CONVERSION_8MS     = 0x7,
  INA226_CONF_AVG_16                   = 0x2
}                              ina226_conf_t;


APP_TIMER_DEF(m_measurement_timer);
static nrf_drv_twi_t           m_twi = NRF_DRV_TWI_INSTANCE(0);
static ina226_measurement_cb_t m_measurement_cb;
static double                  m_current_lsb;
static uint16_t                m_calibration;


static void read(ina226_reg_t reg, uint8_t *buf, uint8_t len) {
  APP_ERROR_CHECK(nrf_drv_twi_tx(&m_twi, INA226_ADDRESS, &reg, 1, true));
  APP_ERROR_CHECK(nrf_drv_twi_rx(&m_twi, INA226_ADDRESS, buf, len));
}

static int16_t read16(ina226_reg_t reg) {
  uint8_t buf[2];
  read(reg, buf, 2);
  return buf[0] << 8 | buf[1];  // buf[0] is the high byte
}

static void write16(uint8_t reg, uint16_t data) {
  uint8_t buf[3] = {reg, (uint8_t) (data >> 8), (uint8_t) (data & 0xFF)};
  APP_ERROR_CHECK(nrf_drv_twi_tx(&m_twi, INA226_ADDRESS, buf, sizeof(buf), false));
}


static void twi_init(void) {
  const nrf_drv_twi_config_t twi_config = {
      .scl                = PIN_SCL,
      .sda                = PIN_SDA,
      .frequency          = NRF_TWI_FREQ_400K,
      .interrupt_priority = APP_IRQ_PRIORITY_LOW,
      .clear_bus_init     = false
  };

  APP_ERROR_CHECK(nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL));
  nrf_drv_twi_enable(&m_twi);
}


static void on_measurement_timer(void *ctx) {
  int16_t raw_shunt_voltage = read16(INA226_REG_SHUNT_VOLTAGE);
  int16_t raw_bus_voltage   = read16(INA226_REG_BUS_VOLTAGE);
  int16_t raw_shunt_current = -read16(INA226_REG_SHUNT_CURRENT);  // Use minus sign to get charging currents to be positive
  double  shunt_voltage_mv  = raw_shunt_voltage * INA226_SHUNT_VOLTAGE_LSB_UV / 1000;
  double  bus_voltage_mv    = raw_bus_voltage * INA226_BUS_VOLTAGE_LSB_MV;
  double  shunt_current_A   = raw_shunt_current * m_current_lsb;
  m_measurement_cb(raw_shunt_voltage, shunt_voltage_mv, shunt_current_A, bus_voltage_mv);
}

static void calibrate(double shunt_resistance, double i_max_expected) {
  m_current_lsb = i_max_expected / 32767;
  m_calibration = (uint16_t) (0.00512 / (m_current_lsb * shunt_resistance));
}


void ina226_init(uint32_t measurement_interval_ms, double shunt_resistance, double i_max_expected, ina226_measurement_cb_t callback) {
  m_measurement_cb = callback;

  twi_init();

  // Initialize INA226. Measure shunt and bus voltage, conversion time 8ms with 16 sample averaging => new measurement available every ~128ms
  uint16_t config = INA226_CONF_SHUNT_AND_BUS_CONTINUOUS |
                    INA226_CONF_SHUNT_CONVERSION_8MS << 3 |
                    INA226_CONF_BUS_CONVERSION_8MS << 6 |
                    INA226_CONF_AVG_16 << 9;
  write16(INA226_REG_CONFIG, config);

  calibrate(shunt_resistance, i_max_expected);
  write16(INA226_REG_CALIBRATION, m_calibration);

  app_timer_create(&m_measurement_timer, APP_TIMER_MODE_REPEATED, on_measurement_timer);
  app_timer_start(m_measurement_timer, APP_TIMER_TICKS(measurement_interval_ms), NULL);
}
