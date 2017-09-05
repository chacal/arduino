#include "ble_data_service.h"
#include <nrf_ble_qwr.h>
#include <nrf_log.h>
#include <app_error.h>
#include <ble_gatts.h>

static uint8_t                       m_rx_buffer[2 * DATA_SERVICE_RX_CHARACTERISTIC_MAX_LEN];
static nrf_ble_qwr_t                 m_qwr;
static uint16_t                      m_data_service_service_handle;
static ble_uuid_t                    m_data_service_ble_uuid;
static ble_gatts_char_handles_t      m_data_service_rx_char_handles;
static ble_data_service_rx_handler_t m_rx_handler = NULL;


//
//  QWR module handlers, these are really not used
//
static uint16_t qwr_event_handler(struct nrf_ble_qwr_t *p_qwr, nrf_ble_qwr_evt_t *p_evt) {
  NRF_LOG_WARNING("qwr_event_handler() called! This should not happen.\n");
  return BLE_GATT_STATUS_SUCCESS;
}

static void qwr_error_handler(uint32_t nrf_error) {
  NRF_LOG_ERROR("---- QWR error %d ----\n", nrf_error);
}


//
//  Initialization functions
//
static void qwr_init() {
  memset(&m_rx_buffer, 0, sizeof(m_rx_buffer));

  nrf_ble_qwr_init_t qwr_init;
  qwr_init.callback         = qwr_event_handler;
  qwr_init.error_handler    = qwr_error_handler;
  qwr_init.mem_buffer.p_mem = m_rx_buffer;
  qwr_init.mem_buffer.len   = sizeof(m_rx_buffer);

  ret_code_t err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
  APP_ERROR_CHECK(err_code);
}


static void ble_service_init() {
  NRF_LOG_INFO("BLE data service. Using %d byte RX buffer.\n", sizeof(m_rx_buffer));
  ret_code_t    err_code;
  ble_uuid128_t base_uuid = DATA_SERVICE_BASE_UUID;

  err_code = sd_ble_uuid_vs_add(&base_uuid, &m_data_service_ble_uuid.type);
  m_data_service_ble_uuid.uuid = DATA_SERVICE_SERVICE_UUID;
  APP_ERROR_CHECK(err_code);

  err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &m_data_service_ble_uuid, &m_data_service_service_handle);
  APP_ERROR_CHECK(err_code);
}


static void ble_characteristic_init() {
  ble_add_char_params_t add_char_params;
  ret_code_t            err_code;
  memset(&add_char_params, 0, sizeof(add_char_params));

  add_char_params.uuid                     = DATA_SERVICE_RX_CHARACTERISTIC_UUID;
  add_char_params.uuid_type                = m_data_service_ble_uuid.type;
  add_char_params.max_len                  = DATA_SERVICE_RX_CHARACTERISTIC_MAX_LEN;
  add_char_params.init_len                 = 0;
  add_char_params.is_var_len               = true;
  add_char_params.char_props.read          = true;
  add_char_params.char_props.write         = true;
  add_char_params.char_props.write_wo_resp = true;
  add_char_params.write_access             = SEC_OPEN;

  err_code = characteristic_add(m_data_service_service_handle, &add_char_params, &m_data_service_rx_char_handles);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_ble_qwr_attr_register(&m_qwr, m_data_service_rx_char_handles.value_handle);
  APP_ERROR_CHECK(err_code);
}


void ble_data_service_init(ble_data_service_rx_handler_t rx_handler) {
  if(rx_handler != NULL) {
    m_rx_handler = rx_handler;
  }
  qwr_init();
  ble_service_init();
  ble_characteristic_init();
}


//
//  BLE event handlers
//
void handle_write_ble_event(ble_gatts_evt_write_t *p_evt_write) {
  if(p_evt_write->op == BLE_GATTS_OP_WRITE_REQ || p_evt_write->op == BLE_GATTS_OP_WRITE_CMD) {
    m_rx_handler(p_evt_write->data, p_evt_write->len);
  } else if(p_evt_write->op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) {
    uint8_t  buf[DATA_SERVICE_RX_CHARACTERISTIC_MAX_LEN];
    uint16_t len = sizeof(buf);
    nrf_ble_qwr_value_get(&m_qwr, m_data_service_rx_char_handles.value_handle, buf, &len);
    m_rx_handler(buf, len);
  }
}


void ble_data_service_on_ble_evt(ble_evt_t *p_ble_evt) {
  nrf_ble_qwr_on_ble_evt(&m_qwr, p_ble_evt);

  switch(p_ble_evt->header.evt_id) {
    case BLE_GAP_EVT_CONNECTED:
      nrf_ble_qwr_conn_handle_assign(&m_qwr, p_ble_evt->evt.gap_evt.conn_handle);
      break;
    case BLE_GATTS_EVT_WRITE:
      if(m_rx_handler != NULL) {
        handle_write_ble_event(&p_ble_evt->evt.gatts_evt.params.write);
      }
      break;
    default:
      break;
  }
}