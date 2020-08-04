#include <nrf.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include <nrf_log_default_backends.h>
#include <app_error.h>
#include <cstring>
#include <nrfx_uarte.h>
#include <nrfx_wdt.h>
#include "util.hpp"
#include "packet_queue.hpp"
#include "radio.hpp"


#define FILTERED_MANUFACTURER_ID   0xDADA
#define UART_TX_PIN                13      // Use pin 2 if flashed on bme280 sensor board using SCL pin as UART TX
#define MAX_TX_QUEUE_SIZE          30      // How many received messages are buffered waiting for UART TX before starting to discard oldest ones
#define RX_WATCHDOG_TIMEOUT_MS     30000   // Reset system if no packets have been written to uart in this many ms.

nrfx_uarte_t m_uart = NRFX_UARTE_INSTANCE(0);

packet_queue m_received_packets(MAX_TX_QUEUE_SIZE);


static void uart_send_str(const std::string &str) {
  nrfx_uarte_tx(&m_uart, (uint8_t *) str.c_str(), str.size());
}

static void on_rx_adv_packet(nrf_packet_data adv_packet, int rssi) {
  m_received_packets.push({adv_packet, rssi});
}

static void wdt_init() {
  nrfx_wdt_config_t   config = NRFX_WDT_DEAFULT_CONFIG;
  config.reload_value = RX_WATCHDOG_TIMEOUT_MS;
  APP_ERROR_CHECK(nrfx_wdt_init(&config, nullptr));
  nrfx_wdt_channel_id m_channel_id;
  nrfx_wdt_channel_alloc(&m_channel_id);
  nrfx_wdt_enable();
}

static void wdt_feed() {
  nrfx_wdt_feed();
}

static void process_received_packet(const adv_packet &packet) {
  auto manufacturer_id = packet.manufacturer_id();

  if (manufacturer_id == FILTERED_MANUFACTURER_ID) {
    auto hex_data = util::tohex(packet.data.payload, packet.data.payload_length);
    auto json_msg = R"({"data": ")" + hex_data + R"(", "rssi": )" + std::to_string(packet.rssi) + "}\n";
    uart_send_str(json_msg);
    wdt_feed();
  }
}

static void uart_init() {
  nrfx_uarte_config_t config = NRFX_UARTE_DEFAULT_CONFIG;
  config.pseltxd = UART_TX_PIN;
  APP_ERROR_CHECK(nrfx_uarte_init(&m_uart, &config, nullptr));
}

int main() {
  APP_ERROR_CHECK(NRF_LOG_INIT(nullptr));
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  NRF_LOG_INFO("BT sensor receiver started")

  util::start_clocks();
  uart_init();
  wdt_init();

  radio_init(on_rx_adv_packet);
  radio_rx_start();

  for (;;) {
    if (m_received_packets.process_next_with(process_received_packet)) {
      __WFE();
      __SEV();
      __WFI();
    }
  }
}
