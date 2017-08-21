#include <ble_nus.h>
#include <nrf_log.h>
#include <app_error.h>
#include "ble_serial_link.h"

static ble_nus_t                        m_nus;                                      /**< Structure to identify the Nordic UART Service. */
static ble_serial_link_rx_handler_t     m_rx_handler = NULL;


static void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length) {
  if(m_rx_handler != NULL) {
    m_rx_handler(p_data, length);
  }
}


void ble_serial_link_init(ble_serial_link_rx_handler_t rx_handler) {
  if(rx_handler != NULL) {
    m_rx_handler = rx_handler;
  }

  uint32_t       err_code;
  ble_nus_init_t nus_init;

  memset(&nus_init, 0, sizeof(nus_init));
  nus_init.data_handler = nus_data_handler;

  err_code = ble_nus_init(&m_nus, &nus_init);
  APP_ERROR_CHECK(err_code);
}


void ble_serial_link_on_ble_evt(ble_evt_t *p_ble_evt) {
  ble_nus_on_ble_evt(&m_nus, p_ble_evt);
}


uint32_t ble_serial_link_string_send(uint8_t *p_string, size_t length) {
  if(m_nus.conn_handle != BLE_CONN_HANDLE_INVALID && m_nus.is_notification_enabled && p_string != NULL && length > 0) {
    uint32_t err_code = ble_nus_string_send(&m_nus, p_string, length);
    APP_ERROR_CHECK(err_code);
    return NRF_SUCCESS;
  }

  return NRF_ERROR_INVALID_STATE;
}