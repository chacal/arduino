#include <app_error.h>
#include <nrf_log.h>
#include <nrf_drv_twi.h>
#include "bmp180.h"
#include <math.h>
#include <app_timer.h>

#define BMP180_ADDRESS                 0x77
#define BMP_180_DEVICE_ID              0x55
#define BMP180_MEASUREMENT_TIME_MS        5
#define CALIBRATION_BYTE_COUNT           22
#define APP_TIMER_PRESCALER               0

typedef enum {
  BMP180_REG_ID                = 0xD0,
  BMP180_REG_FIRST_CALIBRATION = 0xAA,
  BMP180_REG_CONTROL           = 0xF4,
  BMP180_REG_RESULT            = 0xF6
} bmp180_reg_t;

typedef enum {
  BMP180_CMD_TEMPERATURE = 0x2E,
  BMP180_CMD_PRESSURE    = 0x34
} bmp180_command_t;

static int16_t  AC1, AC2, AC3, VB1, VB2, MB, MC, MD;
static uint16_t AC4, AC5, AC6;
static double   b1, c3, c4, c5, c6, mc, md, x0, x1, x2, y_0, y_1, y2, p0, p1, p2;  // y0 & y1 are already defined in math.h

static nrf_drv_twi_t *m_twi;
APP_TIMER_DEF(m_measurement_timer);


__STATIC_INLINE int16_t to_int16_t(uint8_t *buf) {
  return (((int16_t) buf[0]) << 8 | buf[1]);
}

__STATIC_INLINE uint16_t to_uint16_t(uint8_t *buf) {
  return (uint16_t) to_int16_t(buf);
}

static void read(uint8_t reg, uint8_t *buf, uint8_t len) {
  APP_ERROR_CHECK(nrf_drv_twi_tx(m_twi, BMP180_ADDRESS, &reg, 1, true));
  APP_ERROR_CHECK(nrf_drv_twi_rx(m_twi, BMP180_ADDRESS, buf, len));
}

static void write(uint8_t reg, uint8_t data) {
  uint8_t buf[2] = {reg, data};
  APP_ERROR_CHECK(nrf_drv_twi_tx(m_twi, BMP180_ADDRESS, buf, sizeof(buf), false));
}

static void calibrate() {
  uint8_t calib[CALIBRATION_BYTE_COUNT];
  read(BMP180_REG_FIRST_CALIBRATION, calib, sizeof(calib));

  AC1 = to_int16_t(&calib[0]);
  AC2 = to_int16_t(&calib[2]);
  AC3 = to_int16_t(&calib[4]);
  AC4 = to_uint16_t(&calib[6]);
  AC5 = to_uint16_t(&calib[8]);
  AC6 = to_uint16_t(&calib[10]);
  VB1 = to_int16_t(&calib[12]);
  VB2 = to_int16_t(&calib[14]);
  MB  = to_int16_t(&calib[16]);
  MC  = to_int16_t(&calib[18]);
  MD  = to_int16_t(&calib[20]);

  c3  = 160.0 * pow(2, -15) * AC3;
  c4  = pow(10, -3) * pow(2, -15) * AC4;
  b1  = pow(160, 2) * pow(2, -30) * VB1;
  c5  = (pow(2, -15) / 160) * AC5;
  c6  = AC6;
  mc  = (pow(2, 11) / pow(160, 2)) * MC;
  md  = MD / 160.0;
  x0  = AC1;
  x1  = 160.0 * pow(2, -13) * AC2;
  x2  = pow(160, 2) * pow(2, -25) * VB2;
  y_0 = c4 * pow(2, 15);
  y_1 = c4 * c3;
  y2  = c4 * b1;
  p0  = (3791.0 - 8.0) / 1600.0;
  p1  = 1.0 - 7357.0 * pow(2, -20);
  p2  = 3038.0 * 100.0 * pow(2, -36);
}

static void on_measurement_timer(void *ctx) {
  static double last_temp = 0;

  if((int)ctx == BMP180_MEASUREMENT_TYPE_TEMPERATURE) {
    uint8_t data[2];
    read(BMP180_REG_RESULT, data, 2);

    double tu = (data[0] * 256.0) + data[1];
    double a = c5 * (tu - c6);
    double T = a + (mc / (a + md));
    last_temp = T;
    NRF_LOG_INFO("Temp: %d\n", T*1000);

    write(BMP180_REG_CONTROL, BMP180_CMD_PRESSURE);
    app_timer_start(m_measurement_timer, APP_TIMER_TICKS(BMP180_MEASUREMENT_TIME_MS, APP_TIMER_PRESCALER), (void*)BMP180_MEASUREMENT_TYPE_PRESSURE);
  } else if((int)ctx == BMP180_MEASUREMENT_TYPE_PRESSURE) {
    uint8_t data[3];
    read(BMP180_REG_RESULT, data, 3);

    NRF_LOG_INFO("Last temp: %d\n", last_temp);

    double pu = (data[0] * 256.0) + data[1] + (data[2]/256.0);

    double s = last_temp - 25.0;
    double x = (x2 * pow(s,2)) + (x1 * s) + x0;
    double y = (y2 * pow(s,2)) + (y_1 * s) + y_0;
    double z = (pu - x) / y;
    double P = (p2 * pow(z,2)) + (p1 * z) + p0;
    NRF_LOG_INFO("Pressure: %d\n", P*10);
  }
}

void bmp180_init(nrf_drv_twi_t *twi) {
  m_twi = twi;

  uint8_t bmp180_id;
  read(BMP180_REG_ID, &bmp180_id, 1);

  if(bmp180_id == BMP_180_DEVICE_ID) {
    NRF_LOG_INFO("BMP180 connected.\n")
    calibrate();
  } else {
    NRF_LOG_ERROR("BMP180 not found!\n")
  }

  app_timer_create(&m_measurement_timer, APP_TIMER_MODE_SINGLE_SHOT, on_measurement_timer);
}

void bmp180_get_temp_and_pressure() {
  write(BMP180_REG_CONTROL, BMP180_CMD_TEMPERATURE);
  app_timer_start(m_measurement_timer, APP_TIMER_TICKS(BMP180_MEASUREMENT_TIME_MS, APP_TIMER_PRESCALER), (void*)BMP180_MEASUREMENT_TYPE_TEMPERATURE);
}
