#include <nrf_esb.h>
#include <app_error.h>
#include <sdk_common.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <nrf_drv_uart.h>

#ifdef NRF51

#include <sdk_config_nrf51/sdk_config.h>

#else
#include <sdk_config_nrf52/sdk_config.h>
#endif

#define RX_BASE_ADDR     {0x4E, 0x6F, 0x64, 0x65}  // "Node"
#define RX_PREFIX_ADDR   {0x31}                    // "1"
#define RX_CHANNEL       76
#define UART_TX_PIN      5

nrf_drv_uart_t    m_uart = NRF_DRV_UART_INSTANCE(UART0_INSTANCE_INDEX);
nrf_esb_payload_t rx_payload;


void tohex(char *in, size_t insz, char *out, size_t outsz) {
  char       *pin  = in;
  const char *hex  = "0123456789ABCDEF";
  char       *pout = out;

  for(; pin < in + insz; pout += 2, pin++) {
    pout[0] = hex[(*pin >> 4) & 0xF];
    pout[1] = hex[*pin & 0xF];
    if(pout + 2 - out > outsz) {
      break;
    }
  }

  pout[0] = 0;
}

void uart_send_str(char *str) {
  uint8_t len = (uint8_t) strlen(str);
  nrf_drv_uart_tx(&m_uart, (uint8_t *) str, len);
}

void nrf_esb_event_handler(nrf_esb_evt_t const *p_event) {
  switch(p_event->evt_id) {
    case NRF_ESB_EVENT_RX_RECEIVED:
      if(nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS) {
        char tx_buf[200];
        char hex_buf[50];

        tohex((char *) rx_payload.data, rx_payload.length, hex_buf, sizeof(hex_buf));
        sprintf(tx_buf, "{\"data\": \"%s\", \"rssi\": %d}\n", hex_buf, rx_payload.rssi);

        uart_send_str(tx_buf);
      }
      break;
    default:
      break;
  }
}

void uart_init() {
  nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
  config.pseltxd = UART_TX_PIN;
  APP_ERROR_CHECK(nrf_drv_uart_init(&m_uart, &config, NULL));
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

  nrf_esb_config.payload_length  = 32;
  nrf_esb_config.protocol        = NRF_ESB_PROTOCOL_ESB_DPL;
  nrf_esb_config.bitrate         = NRF_ESB_BITRATE_250KBPS;
  nrf_esb_config.mode            = NRF_ESB_MODE_PRX;
  nrf_esb_config.tx_output_power = NRF_ESB_TX_POWER_4DBM;
  nrf_esb_config.event_handler   = nrf_esb_event_handler;

  APP_ERROR_CHECK(nrf_esb_init(&nrf_esb_config));
  APP_ERROR_CHECK(nrf_esb_set_base_address_0(base_addr_0));
  APP_ERROR_CHECK(nrf_esb_set_prefixes(addr_prefix, sizeof(addr_prefix)));
  APP_ERROR_CHECK(nrf_esb_set_rf_channel(RX_CHANNEL));
}

int main(void) {
  uart_init();
  clocks_init();
  radio_init();
  APP_ERROR_CHECK(nrf_esb_start_rx());

  uart_send_str("Sensor receiver started..\n");

  while(true) {
    __WFE();
  }
}
