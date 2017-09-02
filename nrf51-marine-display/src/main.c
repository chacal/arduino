#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "app_timer.h"
#include "ble_support.h"
#include "ble_data_service.h"
#include "power_manager.h"
#include "display.h"

#define WAKEUP_BUTTON_PIN      18

static void on_data_service_rx(uint8_t *p_data, uint16_t length) {
  NRF_LOG_INFO("Received %d bytes: %s\n", length, (uint32_t)p_data);
  if(length == 1 && p_data[0] == 's') {
    power_manager_shutdown();
  } else if(length <= 3) {
    char temp[length + 1];
    strncpy(temp, (char*)p_data, length);
    temp[length] = '\0';
    display_render_str(temp);
  }
}

static void on_ble_event(ble_evt_t *p_ble_evt) {
  ble_data_service_on_ble_evt(p_ble_evt);

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


int main(void) {
  (void) NRF_LOG_INIT(NULL);
  APP_TIMER_INIT(0, 8, NULL);

  power_manager_init(WAKEUP_BUTTON_PIN);
  ble_support_init(on_ble_event);
  ble_data_service_init(on_data_service_rx);
  ble_support_advertising_init();
  display_init();

  ble_support_advertising_start();

  while(true) {
    power_manage();
  }
}
