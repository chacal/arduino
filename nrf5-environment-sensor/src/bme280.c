#include <app_error.h>
#include <nrf_log.h>
#include <nrf_drv_twi.h>
#include "bme280.h"
#include <math.h>
#include <app_timer.h>
#include <app_util_platform.h>
#include <nrf_drv_gpiote.h>
#include <drivers_nrf/delay/nrf_delay.h>

#include "sdk_config.h"

#define PIN_SDA           2
#define PIN_SCL           3
#define PIN_BME280_GND    4
#define PIN_BME280_POWER  5


#define BME280_ADDRESS                 0x76
#define BME280_DEVICE_ID               0x60
#define BME280_MEASUREMENT_TIME_MS       10
#define BME280_STARTUP_TIME_MS           50

#define BME280_CALIBRATION_BYTE_COUNT    32
#define BME280_TEMP_DIG_LENGTH            6
#define BME280_PRESS_DIG_LENGTH          18
#define BME280_HUM_DIG_1_LENGTH           1
#define BME280_HUM_DIG_2_LENGTH           7

#define BME280_SENSOR_DATA_LENGTH         8


typedef enum {
  BME280_TIMER_POWER_ON,
  BME280_TIMER_START_MEASUREMENTS,
  BME280_TIMER_READ_MEASUREMENTS,
}                              bme280_timer_cmd_t;

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
}                              bme280_reg_t;


APP_TIMER_DEF(m_measurement_timer);
static nrf_drv_twi_t           m_twi = NRF_DRV_TWI_INSTANCE(0);
static uint32_t                m_measurement_interval_ms;
static bme280_measurement_cb_t m_measurement_cb;
uint8_t                        m_dig[BME280_CALIBRATION_BYTE_COUNT];


static void read(uint8_t reg, uint8_t *buf, uint8_t len) {
  APP_ERROR_CHECK(nrf_drv_twi_tx(&m_twi, BME280_ADDRESS, &reg, 1, true));
  APP_ERROR_CHECK(nrf_drv_twi_rx(&m_twi, BME280_ADDRESS, buf, len));
}

static void write(uint8_t reg, uint8_t data) {
  uint8_t buf[2] = {reg, data};
  APP_ERROR_CHECK(nrf_drv_twi_tx(&m_twi, BME280_ADDRESS, buf, sizeof(buf), false));
}


static void calibrate() {
  uint8_t count = 0;

  // Temp. Dig
  read(BME280_REG_TEMP_DIG, &m_dig[count], BME280_TEMP_DIG_LENGTH);
  count += BME280_TEMP_DIG_LENGTH;

  // Pressure Dig
  read(BME280_REG_PRESS_DIG, &m_dig[count], BME280_PRESS_DIG_LENGTH);
  count += BME280_PRESS_DIG_LENGTH;

  // Humidity Dig 1
  read(BME280_REG_HUM_DIG_1, &m_dig[count], BME280_HUM_DIG_1_LENGTH);
  count += BME280_HUM_DIG_1_LENGTH;

  // Humidity Dig 2
  read(BME280_REG_HUM_DIG_2, &m_dig[count], BME280_HUM_DIG_2_LENGTH);
}

// Returns temperature as °C
double calculate_temperature(int32_t raw, int32_t *t_fine) {
  // Code based on calibration algorthim provided by Bosch.
  int32_t  var1, var2, final;
  uint16_t dig_T1 = (m_dig[1] << 8) | m_dig[0];
  int16_t  dig_T2 = (m_dig[3] << 8) | m_dig[2];
  int16_t  dig_T3 = (m_dig[5] << 8) | m_dig[4];
  var1 = ((((raw >> 3) - ((int32_t) dig_T1 << 1))) * ((int32_t) dig_T2)) >> 11;
  var2 = (((((raw >> 4) - ((int32_t) dig_T1)) * ((raw >> 4) - ((int32_t) dig_T1))) >> 12) * ((int32_t) dig_T3)) >> 14;
  *t_fine = var1 + var2;
  final = (*t_fine * 5 + 128) >> 8;
  return final / 100.0;
}

// Returns pressure as hPa == mbar
double calculate_pressure(int32_t raw, int32_t t_fine) {
  // Code based on calibration algorthim provided by Bosch.
  int64_t var1, var2, pressure;
  double  final;

  uint16_t dig_P1 = (m_dig[7] << 8) | m_dig[6];
  int16_t  dig_P2 = (m_dig[9] << 8) | m_dig[8];
  int16_t  dig_P3 = (m_dig[11] << 8) | m_dig[10];
  int16_t  dig_P4 = (m_dig[13] << 8) | m_dig[12];
  int16_t  dig_P5 = (m_dig[15] << 8) | m_dig[14];
  int16_t  dig_P6 = (m_dig[17] << 8) | m_dig[16];
  int16_t  dig_P7 = (m_dig[19] << 8) | m_dig[18];
  int16_t  dig_P8 = (m_dig[21] << 8) | m_dig[20];
  int16_t  dig_P9 = (m_dig[23] << 8) | m_dig[22];

  var1 = (int64_t) t_fine - 128000;
  var2 = var1 * var1 * (int64_t) dig_P6;
  var2 = var2 + ((var1 * (int64_t) dig_P5) << 17);
  var2 = var2 + (((int64_t) dig_P4) << 35);
  var1 = ((var1 * var1 * (int64_t) dig_P3) >> 8) + ((var1 * (int64_t) dig_P2) << 12);
  var1 = (((((int64_t) 1) << 47) + var1)) * ((int64_t) dig_P1) >> 33;
  if(var1 == 0) { return NAN; }                                                         // Don't divide by zero.
  pressure = 1048576 - raw;
  pressure = (((pressure << 31) - var2) * 3125) / var1;
  var1     = (((int64_t) dig_P9) * (pressure >> 13) * (pressure >> 13)) >> 25;
  var2     = (((int64_t) dig_P8) * pressure) >> 19;
  pressure = ((pressure + var1 + var2) >> 8) + (((int64_t) dig_P7) << 4);

  // Convert to hPa (== mbar)
  final = ((uint32_t) pressure) / 256.0 / 100;

  return final;
}

// Returns humidity as %H
double calculate_humidity(int32_t raw, int32_t t_fine) {
  // Code based on calibration algorthim provided by Bosch.
  int32_t var1;
  uint8_t dig_H1 = m_dig[24];
  int16_t dig_H2 = (m_dig[26] << 8) | m_dig[25];
  uint8_t dig_H3 = m_dig[27];
  int16_t dig_H4 = (m_dig[28] << 4) | (0x0F & m_dig[29]);
  int16_t dig_H5 = (m_dig[30] << 4) | ((m_dig[29] >> 4) & 0x0F);
  int8_t  dig_H6 = m_dig[31];

  var1 = (t_fine - ((int32_t) 76800));
  var1 = (((((raw << 14) - (((int32_t) dig_H4) << 20) - (((int32_t) dig_H5) * var1)) +
            ((int32_t) 16384)) >> 15) * (((((((var1 * ((int32_t) dig_H6)) >> 10) * (((var1 *
                                                                                      ((int32_t) dig_H3)) >> 11) + ((int32_t) 32768))) >> 10) +
                                           ((int32_t) 2097152)) *
                                          ((int32_t) dig_H2) + 8192) >> 14));
  var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t) dig_H1)) >> 4));
  var1 = (var1 < 0 ? 0 : var1);
  var1 = (var1 > 419430400 ? 419430400 : var1);
  return ((uint32_t) (var1 >> 12)) / 1024.0;
}


static void read_raw_data(int32_t *result) {
  uint8_t buffer[BME280_SENSOR_DATA_LENGTH];

  // Registers are in order. So we can start at the pressure register and read 8 bytes.
  read(BME280_REG_PRESSURE, buffer, BME280_SENSOR_DATA_LENGTH);

  for(int i = 0; i < BME280_SENSOR_DATA_LENGTH; ++i) {
    result[i] = (int32_t) buffer[i];
  }
}

static void read_all_sensor_values(double *temp, double *pressure, double *humidity) {
  int32_t data[8];
  int32_t t_fine;
  read_raw_data(data);

  uint32_t rawPressure = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
  uint32_t rawTemp     = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
  uint32_t rawHumidity = (data[6] << 8) | data[7];
  *temp     = calculate_temperature(rawTemp, &t_fine);
  *pressure = calculate_pressure(rawPressure, t_fine);
  *humidity = calculate_humidity(rawHumidity, t_fine);
}

static void trigger_measurements() {
  // ctrl_hum register. (ctrl_hum[2:0] = Humidity oversampling rate.)
  uint8_t ctrlHum = 1;   // 1 x humidity oversample

  // ctrl_meas register. (ctrl_meas[7:5] = temperature oversampling rate, ctrl_meas[4:2] = pressure oversampling rate, ctrl_meas[1:0] = mode.)
  uint8_t ctrlMeas = (1 << 5) | (1 << 2) | 1;   // 1 x temp oversample, 1 x pressure oversample, forced mode

  // config register. (config[7:5] = standby time, config[4:2] = filter, ctrl_meas[0] = spi enable.)
  uint8_t config = (0 << 5) | (0 << 2) | 0;  // 0ms normal mode standby time, no IIR filter, disable SPI

  write(BME280_REG_CTRL_HUM, ctrlHum);
  write(BME280_REG_CTRL_MEAS, ctrlMeas);
  write(BME280_REG_CONFIG, config);
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

static void bme280_power_on() {
#ifdef PIN_BME280_GND
  nrf_drv_gpiote_out_config_t bme280_gnd_pin_config = GPIOTE_CONFIG_OUT_SIMPLE(false);
  nrf_drv_gpiote_out_init(PIN_BME280_GND, &bme280_gnd_pin_config);
#endif
  nrf_drv_gpiote_out_config_t bme280_power_pin_config = GPIOTE_CONFIG_OUT_SIMPLE(true);
  nrf_drv_gpiote_out_init(PIN_BME280_POWER, &bme280_power_pin_config);
}

static void bme280_power_off() {
  nrf_drv_gpiote_out_uninit(PIN_BME280_POWER);
#ifdef PIN_BME280_GND
  nrf_drv_gpiote_out_uninit(PIN_BME280_GND);
#endif
}


static void on_measurement_timer(void *ctx) {
  bme280_timer_cmd_t cmd = (bme280_timer_cmd_t) ctx;

  switch(cmd) {
    case BME280_TIMER_POWER_ON:
      bme280_power_on();
      app_timer_start(m_measurement_timer, APP_TIMER_TICKS(BME280_STARTUP_TIME_MS), (void *) BME280_TIMER_START_MEASUREMENTS);
      break;

    case BME280_TIMER_START_MEASUREMENTS:
      twi_init();
      trigger_measurements();
      nrf_drv_twi_uninit(&m_twi);
      app_timer_start(m_measurement_timer, APP_TIMER_TICKS(BME280_MEASUREMENT_TIME_MS), (void *) BME280_TIMER_READ_MEASUREMENTS);
      break;

    case BME280_TIMER_READ_MEASUREMENTS: {
      double temp, pressure, humidity;

      twi_init();
      read_all_sensor_values(&temp, &pressure, &humidity);
      nrf_drv_twi_uninit(&m_twi);
      bme280_power_off();

      m_measurement_cb(temp, pressure, humidity);

      app_timer_start(m_measurement_timer, APP_TIMER_TICKS(m_measurement_interval_ms), (void *) BME280_TIMER_POWER_ON);
    }
      break;
  }
}


void bme280_init(uint32_t measurement_interval_ms, bme280_measurement_cb_t callback) {
  m_measurement_interval_ms = measurement_interval_ms;
  m_measurement_cb          = callback;
  nrf_drv_gpiote_init();

  twi_init();
  bme280_power_on();

  nrf_delay_ms(BME280_STARTUP_TIME_MS);

  uint8_t bme280_id;
  read(BME280_REG_ID, &bme280_id, sizeof(bme280_id));

  if(bme280_id == BME280_DEVICE_ID) {
    NRF_LOG_INFO("BME280 connected.")
    calibrate();
    app_timer_create(&m_measurement_timer, APP_TIMER_MODE_SINGLE_SHOT, on_measurement_timer);
    app_timer_start(m_measurement_timer, APP_TIMER_TICKS(measurement_interval_ms), (void *) BME280_TIMER_POWER_ON);
  } else {
    NRF_LOG_ERROR("BME280 not found!")
  }

  bme280_power_off();
  nrf_drv_twi_uninit(&m_twi);
}
