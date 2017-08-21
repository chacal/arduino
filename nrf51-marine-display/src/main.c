#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "bsp.h"
#include "app_timer.h"
#include "ble_support.h"
#include "ble_serial_link.h"
#include "power_manager.h"


static void on_serial_link_rx(uint8_t *p_data, uint16_t length) {
  NRF_LOG_INFO("Got %d bytes of data\n", length);
}

static void on_app_timer(void* pContext) {
  char test[] = "Hello world!";
  uint32_t error = ble_serial_link_string_send((uint8_t *)test, sizeof(test));
  if(error == NRF_SUCCESS) {
    NRF_LOG_INFO("Sent data!\n");
  }
}

static void serial_tx_start() {
  APP_TIMER_INIT(0, 4, NULL);

  APP_TIMER_DEF(timer);
  uint32_t err_code = app_timer_create(&timer, APP_TIMER_MODE_REPEATED, on_app_timer);
  APP_ERROR_CHECK(err_code);

  err_code = app_timer_start(timer, APP_TIMER_TICKS(500, 0), NULL);
  APP_ERROR_CHECK(err_code);

  NRF_LOG_INFO("App timer started!\n");
}


int main(void) {
  (void) NRF_LOG_INIT(NULL);
  bsp_board_leds_init();

  ble_stack_init(ble_serial_link_on_ble_evt);
  gap_params_init();
  ble_serial_link_init(on_serial_link_rx);
  advertising_init();
  uint32_t err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
  APP_ERROR_CHECK(err_code);
  NRF_LOG_INFO("Advertising started!\n");

  serial_tx_start();

  while(true) {
    power_manage();
  }
}
