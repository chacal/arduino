#include <nrf_ble_qwr.h>
#include <nrf_log.h>
#include <nrf_sdh_ble.h>
#include <app_error.h>

#include "ble_data_service.hpp"


NRF_BLE_QWR_DEF(m_qwr);
static uint8_t                  m_rx_buffer[2 * DATA_SERVICE_RX_CHARACTERISTIC_MAX_LEN];
static uint16_t                 m_service_handle;
static ble_uuid_t               m_ble_uuid;
static ble_gatts_char_handles_t m_rx_char_handles;


namespace ble_data_service {

  static rx_data_handler m_data_handler;

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

    APP_ERROR_CHECK(nrf_ble_qwr_init(&m_qwr, &qwr_init));
  }

  static void ble_service_init() {
    NRF_LOG_INFO("BLE data service. Using %d byte RX buffer.", sizeof(m_rx_buffer));
    ble_uuid128_t base_uuid = DATA_SERVICE_BASE_UUID;

    APP_ERROR_CHECK(sd_ble_uuid_vs_add(&base_uuid, &m_ble_uuid.type));
    m_ble_uuid.uuid = DATA_SERVICE_SERVICE_UUID;

    APP_ERROR_CHECK(sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &m_ble_uuid, &m_service_handle));
  }

  static void ble_characteristic_init() {
    ble_gatts_char_md_t char_md = {0};
    char_md.char_props.write = 1;

    ble_uuid_t char_uuid;
    char_uuid.uuid = DATA_SERVICE_RX_CHARACTERISTIC_UUID;
    char_uuid.type = m_ble_uuid.type;

    ble_gatts_attr_md_t attr_md = {0};
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_LESC_ENC_WITH_MITM(&attr_md.write_perm);
    attr_md.vlen = 1;
    attr_md.vloc = BLE_GATTS_VLOC_STACK;

    ble_gatts_attr_t attr_char_value = {0};
    attr_char_value.p_uuid    = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.max_len   = DATA_SERVICE_RX_CHARACTERISTIC_MAX_LEN;

    APP_ERROR_CHECK(sd_ble_gatts_characteristic_add(m_service_handle, &char_md, &attr_char_value, &m_rx_char_handles));
    APP_ERROR_CHECK(nrf_ble_qwr_attr_register(&m_qwr, m_rx_char_handles.value_handle));
  }

  static void on_ble_event(const ble_evt_t *p_ble_evt, void *ctx) {
    switch (p_ble_evt->header.evt_id) {
      case BLE_GAP_EVT_CONNECTED:
        nrf_ble_qwr_conn_handle_assign(&m_qwr, p_ble_evt->evt.common_evt.conn_handle);
        break;

      case BLE_GATTS_EVT_WRITE: {
        auto p = &p_ble_evt->evt.gatts_evt.params.write;
        if(p->op == BLE_GATTS_OP_WRITE_REQ || p->op == BLE_GATTS_OP_WRITE_CMD) {
          m_data_handler({p->data, p->data + p->len});
        } else if(p->op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) {
          uint8_t  buf[DATA_SERVICE_RX_CHARACTERISTIC_MAX_LEN];
          uint16_t len = sizeof(buf);
          nrf_ble_qwr_value_get(&m_qwr, m_rx_char_handles.value_handle, buf, &len);
          m_data_handler({buf, buf + len});
        }
      }
    }
  }

  void init(const rx_data_handler &data_handler) {
    m_data_handler = data_handler;
    qwr_init();
    ble_service_init();
    ble_characteristic_init();
    NRF_SDH_BLE_OBSERVER(m_data_service_obs, NRF_BLE_QWR_BLE_OBSERVER_PRIO, on_ble_event, NULL);
  }
}