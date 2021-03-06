#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "app_timer.h"
#include "ble_support.h"
#include "ble_data_service.h"
#include "power_manager.h"
#include "display.h"
#include "pb_msg_handler.h"
#include <app_scheduler.h>
#include <peer_manager.h>
#include <ble_conn_state.h>
#include <softdevice/s130/headers/ble_gap.h>
#include <app_gpiote.h>
#include "config.h"
#include "internal_command.h"
#include "marinedisplay.pb.h"
#include "button_handler.h"
#include "status_bar.h"
#include <app_button.h>

#define APP_GPIOTE_MAX_USERS        1


void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info) {
  error_info_t *error_info = (error_info_t*)info;
  NRF_LOG_ERROR("FATAL Code: %d, %s:%d\n", error_info->err_code, (uint32_t)error_info->p_file_name, error_info->line_num);
  NVIC_SystemReset();
}

static void on_data_service_rx(uint8_t *p_data, uint16_t length) {
  NRF_LOG_INFO("Received %d bytes\n", length);
  pb_msg_decode(p_data, length);
}

static void on_ble_event(ble_evt_t *p_ble_evt) {
  ble_data_service_on_ble_evt(p_ble_evt);
  ble_conn_state_on_ble_evt(p_ble_evt);
  pm_on_ble_evt(p_ble_evt);
  status_bar_on_ble_evt(p_ble_evt);

  switch (p_ble_evt->header.evt_id) {
    case BLE_GAP_EVT_CONNECTED:
      SCHED_INT_CMD(DISPLAY_ON);
      break;
    case BLE_GAP_EVT_DISCONNECTED:
      SCHED_INT_CMD(DISPLAY_OFF);
      break;
    default:
      break;
  }
}

static void scheduler_init() {
  APP_SCHED_INIT(APP_SCHEDULER_EVENT_SIZE, APP_SCHEDULER_QUEUE_SIZE);
  NRF_LOG_INFO("App scheduler initialized. RAM usage %d bytes.\n", APP_SCHED_BUF_SIZE(APP_SCHEDULER_EVENT_SIZE, APP_SCHEDULER_QUEUE_SIZE));
}

int main(void) {
  (void) NRF_LOG_INIT(NULL);
  APP_TIMER_INIT(0, 4, NULL);
  APP_GPIOTE_INIT(APP_GPIOTE_MAX_USERS);

  scheduler_init();
  power_manager_init();
  ble_support_init(on_ble_event);
  ble_data_service_init(on_data_service_rx);
  ble_support_advertising_init();
  display_init();
  btn_handler_init();

  ble_support_advertising_start();

  while(true) {
    power_manage();
    app_sched_execute();
  }
}
