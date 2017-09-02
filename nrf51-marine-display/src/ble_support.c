#include <stdint.h>
#include <ble.h>
#include <ble_hci.h>
#include <app_error.h>
#include <bsp.h>
#include <memory.h>
#include <ble_nus.h>
#include <nrf_log.h>
#include <ble_conn_params.h>
#include <app_timer.h>
#include "ble_support.h"
#include "ble_data_service.h"


#define APP_FEATURE_NOT_SUPPORTED       (BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2)      /**< Reply when unsupported features are requested. */

#define DEVICE_NAME                     "MarineDisplay"                             /**< Name of device. Will be included in the advertising data. */

#define CENTRAL_LINK_COUNT              0
#define PERIPHERAL_LINK_COUNT           1
#define TX_POWER_LEVEL                  4                                           /**< Tx power in dBm */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(250, UNIT_1_25_MS)
#define SLAVE_LATENCY                   0
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(1000, 0)                    /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, 0)                    /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define APP_ADV_INTERVAL                MSEC_TO_UNITS(50, UNIT_0_625_MS)            /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      60                                          /**< The advertising timeout (in units of seconds). */
#define APP_ADV_SLOW_INTERVAL           MSEC_TO_UNITS(100, UNIT_0_625_MS)           /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_SLOW_TIMEOUT_IN_SECONDS 0                                           /**< The advertising timeout (in units of seconds). */


static uint16_t          m_conn_handle     = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
static ble_uuid_t        m_adv_uuids[]     = {{DATA_SERVICE_SERVICE_UUID, DATA_SERVICE_UUID_TYPE}};  /**< Universally unique service identifier. */
static ble_evt_handler_t m_ble_evt_handler = NULL;


static void ble_evt_dispatch(ble_evt_t * p_ble_evt);
static void on_ble_evt(ble_evt_t * p_ble_evt);


/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
 *          the device. It also sets the permissions and appearance.
 */
static void gap_params_init() {
  uint32_t                err_code;
  ble_gap_conn_params_t   gap_conn_params;
  ble_gap_conn_sec_mode_t sec_mode;

  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

  err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *) DEVICE_NAME, strlen(DEVICE_NAME));
  APP_ERROR_CHECK(err_code);

  memset(&gap_conn_params, 0, sizeof(gap_conn_params));

  gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
  gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
  gap_conn_params.slave_latency     = SLAVE_LATENCY;
  gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

  err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
  APP_ERROR_CHECK(err_code);
}


static void conn_params_error_handler(uint32_t nrf_error) {
  APP_ERROR_HANDLER(nrf_error);
}


static void conn_params_init(void) {
  uint32_t err_code;
  ble_conn_params_init_t cp_init;

  memset(&cp_init, 0, sizeof(cp_init));

  cp_init.p_conn_params = NULL;
  cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
  cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
  cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
  cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
  cp_init.disconnect_on_fail = true;
  cp_init.error_handler = conn_params_error_handler;

  err_code = ble_conn_params_init(&cp_init);
  APP_ERROR_CHECK(err_code);
}


/**@brief Function for the SoftDevice initialization.
 *
 * @details This function initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init() {
  uint32_t err_code;
  nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

  // Initialize SoftDevice.
  SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

  ble_enable_params_t ble_enable_params;
  err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT, &ble_enable_params);
  APP_ERROR_CHECK(err_code);

  //Check the ram settings against the used number of links
  CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);

  err_code = softdevice_enable(&ble_enable_params);
  APP_ERROR_CHECK(err_code);

  // Subscribe for BLE events.
  err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
  APP_ERROR_CHECK(err_code);

  err_code = sd_ble_gap_tx_power_set(TX_POWER_LEVEL);
  APP_ERROR_CHECK(err_code);
}


static void ble_evt_dispatch(ble_evt_t * p_ble_evt) {
  on_ble_evt(p_ble_evt);
  ble_advertising_on_ble_evt(p_ble_evt);
  ble_conn_params_on_ble_evt(p_ble_evt);
  if(m_ble_evt_handler != NULL) {
    m_ble_evt_handler(p_ble_evt);
  }
}


static void on_ble_evt(ble_evt_t * p_ble_evt) {
  uint32_t err_code;

  switch (p_ble_evt->header.evt_id) {
    case BLE_GAP_EVT_CONNECTED:
      m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
      break; // BLE_GAP_EVT_CONNECTED

    case BLE_GAP_EVT_DISCONNECTED:
      m_conn_handle = BLE_CONN_HANDLE_INVALID;
      break; // BLE_GAP_EVT_DISCONNECTED

    case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
      // Pairing not supported
      err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
      APP_ERROR_CHECK(err_code);
      break; // BLE_GAP_EVT_SEC_PARAMS_REQUEST

    case BLE_GATTS_EVT_SYS_ATTR_MISSING:
      // No system attributes have been stored.
      err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
      APP_ERROR_CHECK(err_code);
      break; // BLE_GATTS_EVT_SYS_ATTR_MISSING

    case BLE_GATTC_EVT_TIMEOUT:
      // Disconnect on GATT Client timeout event.
      err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
      APP_ERROR_CHECK(err_code);
      break; // BLE_GATTC_EVT_TIMEOUT

    case BLE_GATTS_EVT_TIMEOUT:
      // Disconnect on GATT Server timeout event.
      err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
      APP_ERROR_CHECK(err_code);
      break; // BLE_GATTS_EVT_TIMEOUT

    case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST: {
      ble_gatts_evt_rw_authorize_request_t  req;
      ble_gatts_rw_authorize_reply_params_t auth_reply;

      req = p_ble_evt->evt.gatts_evt.params.authorize_request;

      if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID) {
        if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
            (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
            (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL)) {
          if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE) {
            auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
          } else {
            auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
          }
          auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
          err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                     &auth_reply);
          APP_ERROR_CHECK(err_code);
        }
      }
    } break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

    default:
      break;
  }
}


static void on_adv_evt(const ble_adv_evt_t ble_adv_evt) {
}


void ble_support_init(ble_evt_handler_t ble_evt_handler) {
  m_ble_evt_handler = ble_evt_handler;
  ble_stack_init();
  gap_params_init();
  conn_params_init();
}


void ble_support_advertising_init() {
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


void ble_support_advertising_start() {
  uint32_t err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
  APP_ERROR_CHECK(err_code);
  NRF_LOG_INFO("Advertising started!\n");
}