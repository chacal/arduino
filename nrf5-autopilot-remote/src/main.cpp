#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "ble.hpp"
#include "buttons.hpp"

#define DEVICE_NAME         "A100"

typedef struct {
  uint8_t  ttl;
  uint16_t tag;
  uint32_t crc;
  uint8_t  button_id;
  uint8_t  is_long_press;
  uint16_t vcc;
} autopilot_data_t;

static autopilot_data_t m_ap_data = {
  .ttl            = 2,
  .tag            = 'a',
  .crc            = 0,
  .button_id      = 0,
  .is_long_press  = 0,
  .vcc            = 0
};


int main() {
  APP_ERROR_CHECK(NRF_LOG_INIT(nullptr));
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  NRF_LOG_INFO("nRF5 autopilot remote starting..")

  ble_init(DEVICE_NAME);
  ble_sensor_advertising_init(reinterpret_cast<uint8_t *>(&m_ap_data), sizeof(m_ap_data));
  ble_sensor_advertising_start();

  buttons_init();

  NRF_LOG_INFO("Advertising..")

  for (;;) {
    APP_ERROR_CHECK(sd_app_evt_wait());
  }
}