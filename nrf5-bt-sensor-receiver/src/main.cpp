#include <nrf.h>
#include <app_error.h>
#include <ble_gap.h>
#include <string.h>
#include <nrf_drv_uart.h>
#include "util.hpp"
#include <queue>

extern "C" {
#include "radio.h"
}

#define FILTERED_MANUFACTURER_ID   0xDADA
#define UART_TX_PIN                13      // Use pin 2 if flashed on bme280 sensor board using SCL pin as UART TX
#define MAX_TX_QUEUE_SIZE          30      // How many received messages are buffered waiting for UART TX before starting to discard oldest ones

// Can't use NRF_DRV_UART_INSTANCE macro here due to compile error on C++, see: https://devzone.nordicsemi.com/f/nordic-q-a/18616/can-t-instantiate-uart-driver
nrf_drv_uart_t m_uart = {
    .reg          = {(NRF_UARTE_Type *) NRF_DRV_UART_PERIPHERAL(UART0_INSTANCE_INDEX)},
    .drv_inst_idx = UART0_INSTANCE_INDEX,
};

std::queue<std::string> m_tx_queue;


void uart_send_str(const std::string &str) {
  nrf_drv_uart_tx(&m_uart, (uint8_t *) str.c_str(), str.size());
}

static void on_rx_adv_packet(nrf_radio_packet_t adv_packet, int rssi) {
  auto res = Util::getAdvPacketField(BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, &adv_packet);

  if(res) {
    uint16_t manufacturer_id = (res.value()[0] << 8) | res.value()[1];  // First two bytes are the manufacturer ID
    if(manufacturer_id == FILTERED_MANUFACTURER_ID) {
      while(m_tx_queue.size() >= MAX_TX_QUEUE_SIZE) {
        m_tx_queue.pop();
      }

      auto hex_data = Util::tohex(adv_packet.payload, adv_packet.payload_length);
      auto json_msg = R"({"data": ")" + hex_data + R"(", "rssi": )" + std::to_string(rssi) + "}\n";
      m_tx_queue.push(json_msg);
    }
  }
}

static void send_buffered_messages() {
  while(!m_tx_queue.empty()) {
    uart_send_str(m_tx_queue.front());
    m_tx_queue.pop();
  }
}

void uart_init() {
  nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
  config.pseltxd = UART_TX_PIN;
  APP_ERROR_CHECK(nrf_drv_uart_init(&m_uart, &config, NULL));
}

int main() {
  Util::startClocks();
  uart_init();

  uart_send_str("BT sensor receiver started\n");

  radio_init(on_rx_adv_packet);
  radio_rx_start();

  for(;;) {
    send_buffered_messages();
    __WFE();
    __SEV();
    __WFI();
  }
}
