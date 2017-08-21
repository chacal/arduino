#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "bsp.h"
#include "ble_nus.h"
#include "ble_advertising.h"
#include "app_timer.h"
#include "ble_support.h"


static ble_nus_t                        m_nus;                                      /**< Structure to identify the Nordic UART Service. */


static void ble_evt_dispatch(ble_evt_t * p_ble_evt) {
  ble_nus_on_ble_evt(&m_nus, p_ble_evt);
}


/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_nus    Nordic UART Service structure.
 * @param[in] p_data   Data to be send to UART module.
 * @param[in] length   Length of the data.
 */
static void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length) {
  NRF_LOG_INFO("Got %d bytes of data\n", length);
}


/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void) {
  uint32_t       err_code;
  ble_nus_init_t nus_init;

  memset(&nus_init, 0, sizeof(nus_init));
  nus_init.data_handler = nus_data_handler;

  err_code = ble_nus_init(&m_nus, &nus_init);
  APP_ERROR_CHECK(err_code);
}



static void power_manage(void) {
  uint32_t err_code = sd_app_evt_wait();
  APP_ERROR_CHECK(err_code);
}


static void on_app_timer(void* pContext) {
  if(m_nus.conn_handle != BLE_CONN_HANDLE_INVALID && m_nus.is_notification_enabled) {
    char test[] = "Hello world!";

    uint32_t err_code = ble_nus_string_send(&m_nus, (uint8_t *)test, sizeof(test));
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("Sent data!\n");
  }
}

static void nus_transfer_start() {
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

  ble_stack_init(ble_evt_dispatch);
  gap_params_init();
  services_init();
  advertising_init();
  uint32_t err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
  APP_ERROR_CHECK(err_code);
  NRF_LOG_INFO("Advertising started!\n");

  nus_transfer_start();

  while(true) {
    power_manage();
  }
}
