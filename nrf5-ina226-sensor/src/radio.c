#include <nrf_esb.h>
#include <app_error.h>
#include <nrf_esb.h>
#include "nrf_log.h"

#include "radio.h"

#define TX_BASE_ADDR     {0x4E, 0x6F, 0x64, 0x65}  // "Node"
#define TX_PREFIX_ADDR   {0x31}                    // "1"
#define RADIO_CHANNEL    76


void nrf_esb_event_handler(nrf_esb_evt_t const *p_event) {
  switch(p_event->evt_id) {
    case NRF_ESB_EVENT_TX_SUCCESS:
      NRF_LOG_INFO("Tx success!")
      break;
    default:
      NRF_LOG_INFO("Got event: %d", p_event->evt_id)
      break;
  }
}

void radio_init() {
  uint8_t          base_addr_0[4] = TX_BASE_ADDR;
  uint8_t          addr_prefix[1] = TX_PREFIX_ADDR;
  nrf_esb_config_t nrf_esb_config = NRF_ESB_DEFAULT_CONFIG;

  nrf_esb_config.payload_length  = 32;
  nrf_esb_config.protocol        = NRF_ESB_PROTOCOL_ESB_DPL;
  nrf_esb_config.bitrate         = NRF_ESB_BITRATE_250KBPS;
  nrf_esb_config.mode            = NRF_ESB_MODE_PTX;
  nrf_esb_config.tx_output_power = NRF_ESB_TX_POWER_4DBM;
  nrf_esb_config.event_handler   = nrf_esb_event_handler;

  APP_ERROR_CHECK(nrf_esb_init(&nrf_esb_config));
  APP_ERROR_CHECK(nrf_esb_set_base_address_0(base_addr_0));
  APP_ERROR_CHECK(nrf_esb_set_prefixes(addr_prefix, sizeof(addr_prefix)));
  APP_ERROR_CHECK(nrf_esb_set_rf_channel(RADIO_CHANNEL));
}

void radio_send(uint8_t* data, uint8_t len) {
  nrf_esb_payload_t payload;
  memcpy(payload.data, data, len);
  payload.length = len;
  nrf_esb_write_payload(&payload);
}
