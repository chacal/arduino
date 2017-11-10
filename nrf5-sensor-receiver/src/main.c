#include <nrf_esb.h>
#include <app_error.h>
#include <sdk_common.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>

#define RX_BASE_ADDR     {0x4E, 0x6F, 0x64, 0x65}  // "Node"
#define RX_PREFIX_ADDR   {0x31}                    // "1"
#define RX_CHANNEL       76

nrf_esb_payload_t rx_payload;


void nrf_esb_event_handler(nrf_esb_evt_t const *p_event) {
  switch(p_event->evt_id) {
    case NRF_ESB_EVENT_RX_RECEIVED:
      if(nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS) {
        NRF_LOG_INFO("RX: %d bytes, pipe %d, rssi %d\n", rx_payload.length, rx_payload.pipe, rx_payload.rssi);
        NRF_LOG_HEXDUMP_INFO(rx_payload.data, rx_payload.length);
      }
      break;
    default:
      break;
  }
}


void clocks_init(void) {
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_HFCLKSTART    = 1;

  while(NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}


void radio_init(void) {
  uint8_t          base_addr_0[4] = RX_BASE_ADDR;
  uint8_t          addr_prefix[1] = RX_PREFIX_ADDR;
  nrf_esb_config_t nrf_esb_config = NRF_ESB_DEFAULT_CONFIG;

  nrf_esb_config.payload_length     = 32;
  nrf_esb_config.protocol           = NRF_ESB_PROTOCOL_ESB_DPL;
  nrf_esb_config.bitrate            = NRF_ESB_BITRATE_250KBPS;
  nrf_esb_config.mode               = NRF_ESB_MODE_PRX;
  nrf_esb_config.event_handler      = nrf_esb_event_handler;
  nrf_esb_config.selective_auto_ack = true;

  APP_ERROR_CHECK(nrf_esb_init(&nrf_esb_config));
  APP_ERROR_CHECK(nrf_esb_set_base_address_0(base_addr_0));
  APP_ERROR_CHECK(nrf_esb_set_prefixes(addr_prefix, sizeof(addr_prefix)));
  APP_ERROR_CHECK(nrf_esb_set_rf_channel(RX_CHANNEL));
}


int main(void) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));

  clocks_init();
  radio_init();
  APP_ERROR_CHECK(nrf_esb_start_rx());

  NRF_LOG_INFO("Sensor receiver started..\n");

  while(true) {
    __WFE();
  }
}
