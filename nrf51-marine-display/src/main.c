#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "app_timer.h"
#include "ble_support.h"
#include "ble_serial_link.h"
#include "power_manager.h"
#include "display.h"

#define WAKEUP_BUTTON_PIN      18


static void on_serial_link_rx(uint8_t *p_data, uint16_t length) {
  NRF_LOG_INFO("Got %d bytes of data\n", length);
  if(length == 1 && p_data[0] == 's') {
    power_manager_shutdown();
  } else if(length <= 3) {
    char temp[length + 1];
    strncpy(temp, p_data, length);
    temp[length] = '\0';
    display_render_str(temp);
  }
}

static void on_ble_event(ble_evt_t *p_ble_evt) {
  ble_serial_link_on_ble_evt(p_ble_evt);
  switch (p_ble_evt->header.evt_id) {
    case BLE_GAP_EVT_CONNECTED:
      display_on();
      break;
    case BLE_GAP_EVT_DISCONNECTED:
      display_off();
      break;
    default:
      break;
  }
}

static void on_app_timer(void* pContext) {
  char test[] = "Hello world!";
  uint32_t error = ble_serial_link_string_send((uint8_t *)test, sizeof(test));
  if(error == NRF_SUCCESS) {
    NRF_LOG_INFO("Sent data!\n");
  }
}

static void serial_tx_start() {
  APP_TIMER_DEF(timer);
  uint32_t err_code = app_timer_create(&timer, APP_TIMER_MODE_REPEATED, on_app_timer);
  APP_ERROR_CHECK(err_code);

  err_code = app_timer_start(timer, APP_TIMER_TICKS(500, 0), NULL);
  APP_ERROR_CHECK(err_code);

  NRF_LOG_INFO("App timer started!\n");
}


int main(void) {
  (void) NRF_LOG_INIT(NULL);
  APP_TIMER_INIT(0, 8, NULL);

  power_manager_init(WAKEUP_BUTTON_PIN);
  ble_support_init(on_ble_event);
  ble_serial_link_init(on_serial_link_rx);
  ble_support_advertising_init();
  display_init();

  ble_support_advertising_start();
//  serial_tx_start();

  while(true) {
    power_manage();
  }
}
