#define NRF_LOG_DEFAULT_LEVEL 4

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "ble.hpp"
#include "buttons.hpp"
#include "vcc_measurement.hpp"

#define DEVICE_NAME         "A100"

#pragma pack(1)

typedef struct {
  uint8_t  ttl;
  uint16_t tag;
  uint32_t crc;
  uint8_t  button_id;
  uint8_t  is_long_press;
  uint16_t message_counter;
  uint16_t vcc;
} autopilot_data_t;

static autopilot_data_t m_ap_data = {
  .ttl             = 2,
  .tag             = 'a',
  .crc             = 0,
  .button_id       = 0,
  .is_long_press   = 0,
  .message_counter = 0,
  .vcc             = 0
};

void on_button_push(button_id btn_id, bool is_long_press) {
  sample_vcc();
}

void on_button_release(button_id btn_id, bool is_long_press) {
  NRF_LOG_DEBUG("Button ID %d pressed. Long press: %d", btn_id, is_long_press)
  m_ap_data.button_id       = btn_id;
  m_ap_data.is_long_press   = is_long_press;
  m_ap_data.message_counter = m_ap_data.message_counter + 1;
  m_ap_data.crc             = 0;  // Reset CRC to 0 before calculating new CRC
  m_ap_data.crc             = ble_sensor_advertising_crc32(reinterpret_cast<uint8_t *>(&m_ap_data), sizeof(m_ap_data));

  ble_sensor_advertising_init(reinterpret_cast<uint8_t *>(&m_ap_data), sizeof(m_ap_data));
  ble_sensor_advertising_start();
}

void on_vcc_measurement(uint16_t vcc) {
  NRF_LOG_DEBUG("VCC: %d", vcc)
  m_ap_data.vcc = vcc;
}

static void power_manage(void) {
  // Always clear FPU IRQs to allow CPU to sleep. See: https://devzone.nordicsemi.com/question/87838/high-power-consumption-when-using-fpu/
  __set_FPSCR(__get_FPSCR() & ~(0x0000009F));
  (void) __get_FPSCR();
  NVIC_ClearPendingIRQ(FPU_IRQn);
  APP_ERROR_CHECK(sd_app_evt_wait());
}

int main() {
  APP_ERROR_CHECK(NRF_LOG_INIT(nullptr));
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  NRF_LOG_INFO("nRF5 autopilot remote starting..")

  ble_init(DEVICE_NAME);
  buttons_init(on_button_push, on_button_release);
  vcc_measurement_init(on_vcc_measurement);

  for (;;) {
    power_manage();
  }
}