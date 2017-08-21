#include <stdint.h>
#include <ble.h>
#include <ble_hci.h>
#include <app_error.h>
#include <bsp.h>
#include "ble_support.h"

#define APP_FEATURE_NOT_SUPPORTED       (BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2)        /**< Reply when unsupported features are requested. */

#define CENTRAL_LINK_COUNT              0
#define PERIPHERAL_LINK_COUNT           1
#define TX_POWER_LEVEL                  4                                           /**< Tx power in dBm */

static uint16_t                         m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */


/**@brief Function for the SoftDevice initialization.
 *
 * @details This function initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init(ble_evt_handler_t ble_evt_handler) {
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
  err_code = softdevice_ble_evt_handler_set(ble_evt_handler);
  APP_ERROR_CHECK(err_code);

  err_code = sd_ble_gap_tx_power_set(TX_POWER_LEVEL);
  APP_ERROR_CHECK(err_code);
}


/**@brief Function for the application's SoftDevice event handler.
 *
 * @param[in] p_ble_evt SoftDevice event.
 */
void on_ble_evt(ble_evt_t * p_ble_evt) {
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

    case BLE_EVT_USER_MEM_REQUEST:
      err_code = sd_ble_user_mem_reply(p_ble_evt->evt.gattc_evt.conn_handle, NULL);
      APP_ERROR_CHECK(err_code);
      break; // BLE_EVT_USER_MEM_REQUEST

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
