#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "bsp.h"
#include "ble_nus.h"
#include "ble_advertising.h"
#include "app_timer.h"
#include "ble_support.h"


#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  /**< UUID type for the Nordic UART Service (vendor specific). */

#define APP_ADV_INTERVAL                MSEC_TO_UNITS(50, UNIT_0_625_MS)            /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      60                                          /**< The advertising timeout (in units of seconds). */
#define APP_ADV_SLOW_INTERVAL           MSEC_TO_UNITS(100, UNIT_0_625_MS)           /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_SLOW_TIMEOUT_IN_SECONDS 0                                           /**< The advertising timeout (in units of seconds). */

static ble_nus_t                        m_nus;                                      /**< Structure to identify the Nordic UART Service. */
static ble_uuid_t                       m_adv_uuids[] = {{BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}};  /**< Universally unique service identifier. */


static void ble_evt_dispatch(ble_evt_t * p_ble_evt) {
  ble_nus_on_ble_evt(&m_nus, p_ble_evt);
  on_ble_evt(p_ble_evt);
  ble_advertising_on_ble_evt(p_ble_evt);
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


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(const ble_adv_evt_t ble_adv_evt) {
}


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void) {
  uint32_t               err_code;
  ble_advdata_t          advdata;
  ble_advdata_t          scanrsp;
  ble_adv_modes_config_t options;

  // Build advertising data struct to pass into @ref ble_advertising_init.
  memset(&advdata, 0, sizeof(advdata));
  advdata.name_type          = BLE_ADVDATA_FULL_NAME;
  advdata.include_appearance = false;
  advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

  memset(&scanrsp, 0, sizeof(scanrsp));
  scanrsp.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
  scanrsp.uuids_complete.p_uuids  = m_adv_uuids;

  memset(&options, 0, sizeof(options));
  options.ble_adv_fast_enabled  = true;
  options.ble_adv_fast_interval = APP_ADV_INTERVAL;
  options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;
  options.ble_adv_slow_enabled  = true;
  options.ble_adv_slow_interval = APP_ADV_SLOW_INTERVAL;
  options.ble_adv_slow_timeout  = APP_ADV_SLOW_TIMEOUT_IN_SECONDS;

  err_code = ble_advertising_init(&advdata, &scanrsp, &options, on_adv_evt, NULL);
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
