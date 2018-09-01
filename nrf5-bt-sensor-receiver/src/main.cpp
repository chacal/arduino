#include <nrf.h>
#include <app_error.h>
#include <ble_gap.h>
#include <string.h>
#include <nrf_drv_uart.h>
#include "util.hpp"

extern "C" {
#include "radio.h"
}

#define FILTERED_MANUFACTURER_ID   0xDADA
#define UART_TX_PIN                13      // Use pin 2 if flashed on bme280 sensor board using SCL pin as UART TX

// Can't use NRF_DRV_UART_INSTANCE macro here due to compile error on C++, see: https://devzone.nordicsemi.com/f/nordic-q-a/18616/can-t-instantiate-uart-driver
nrf_drv_uart_t m_uart = {
    .reg          = {(NRF_UARTE_Type *) NRF_DRV_UART_PERIPHERAL(UART0_INSTANCE_INDEX)},
    .drv_inst_idx = UART0_INSTANCE_INDEX,
};

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

static void on_rx_adv_packet(nrf_radio_packet_t adv_packet, int rssi) {
  adv_data_t manuf_data;

  uint32_t res = util_adv_report_parse(BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, &adv_packet, &manuf_data);

  if(res == NRF_SUCCESS) {
    uint16_t *manufacturer_id = (uint16_t *) &manuf_data.data[0];  // First two bytes are the manufacturer ID
    if(*manufacturer_id == FILTERED_MANUFACTURER_ID) {
      char tx_buf[250];
      char hex_buf[80];

      tohex((char *) adv_packet.payload, adv_packet.payload_length, hex_buf, sizeof(hex_buf));
      sprintf(tx_buf, "{\"data\": \"%s\", \"rssi\": %d}\n", hex_buf, rssi);

      uart_send_str(tx_buf);
    }
  }
}

void uart_init() {
  nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
  config.pseltxd = UART_TX_PIN;
  APP_ERROR_CHECK(nrf_drv_uart_init(&m_uart, &config, NULL));
}

int main(void) {
  util_start_clocks();
  uart_init();

  char msg[] = "BT sensor receiver started\n";
  uart_send_str(msg);

  radio_init(on_rx_adv_packet);
  radio_rx_start();

  for(;;) {
    __WFE();
    __SEV();
    __WFI();
  }
}
