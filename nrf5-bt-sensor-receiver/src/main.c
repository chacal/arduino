#include <nrf.h>
#include <app_error.h>
#include <ble_gap.h>
#include <string.h>
#include <nrf_drv_uart.h>
#include "util.h"
#include "radio.h"

/*
 * NOTE! This project still uses SDK v12 instead of SDK v14. This means that bootloader & softdevice from
 * nrf52-ble-bootloader won't work! Instead erase, flash softdevice and flash app using this project's
 * Makefile.
 */

#ifdef NRF51

#include <sdk_config_nrf51/sdk_config.h>

#else
#include <sdk_config_nrf52/sdk_config.h>
#endif


#define FILTERED_MANUFACTURER_ID   0xDADA
#define UART_TX_PIN                13      // Use pin 2 if flashed on bme280 sensor board using SCL pin as UART TX

nrf_drv_uart_t m_uart = NRF_DRV_UART_INSTANCE(UART0_INSTANCE_INDEX);


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
  radio_init(on_rx_adv_packet);
  radio_rx_start();

  uart_send_str("BT sensor receiver started\n");

  for(;;) {
    __WFE();
    __WFI();
  }
}
