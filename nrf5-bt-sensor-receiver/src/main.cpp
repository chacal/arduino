#include <nrf.h>
#include <app_error.h>
#include <ble_gap.h>
#include <string.h>
#include <nrf_drv_uart.h>
#include "util.hpp"
#include "packet_queue.hpp"

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

packet_queue m_received_packets(MAX_TX_QUEUE_SIZE);


static void uart_send_str(const std::string &str) {
  nrf_drv_uart_tx(&m_uart, (uint8_t *) str.c_str(), str.size());
}

static void on_rx_adv_packet(nrf_packet_data adv_packet, int rssi) {
  m_received_packets.push({adv_packet, rssi});
}

static void process_received_packet(const adv_packet &packet) {
  auto manufacturer_id = packet.manufacturer_id();

  if(manufacturer_id == FILTERED_MANUFACTURER_ID) {
    auto hex_data = util::tohex(packet.data.payload, packet.data.payload_length);
    auto json_msg = R"({"data": ")" + hex_data + R"(", "rssi": )" + std::to_string(packet.rssi) + "}\n";
    uart_send_str(json_msg);
  }
}

static void uart_init() {
  nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;
  config.pseltxd = UART_TX_PIN;
  APP_ERROR_CHECK(nrf_drv_uart_init(&m_uart, &config, NULL));
}

int main() {
  util::start_clocks();
  uart_init();

  uart_send_str("BT sensor receiver started\n");

  radio_init(on_rx_adv_packet);
  radio_rx_start();

  for(;;) {
    if(m_received_packets.process_next_with(process_received_packet)) {
      __WFE();
      __SEV();
      __WFI();
    }
  }
}
