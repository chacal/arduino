#include <nrf.h>
#include <nrf_error.h>
#include "util.hpp"

void Util::startClocks() {
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_HFCLKSTART    = 1;
  while(NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}

std::optional<std::vector<uint8_t>> Util::getAdvPacketField(uint8_t adv_type, nrf_radio_packet_t *adv_report_packet) {
  uint8_t *p_data         = &adv_report_packet->payload[6];  // Skip first 6 bytes of adv report (they are a BLE MAC address)
  int     adv_data_length = adv_report_packet->payload_length - 6;

  int index = 0;
  while(index < adv_data_length - 1) {
    uint8_t field_length = p_data[index];
    uint8_t field_type   = p_data[index + 1];

    if(field_type == adv_type) {
      uint8_t *field_data       = &p_data[index + 2];
      int     field_data_length = field_length - 1;
      return {{field_data, field_data + field_data_length}};
    }
    index += field_length + 1;
  }

  return std::nullopt;
}


std::string Util::tohex(uint8_t *in, size_t insz) {
  const char *hex = "0123456789ABCDEF";
  std::string out;

  for(size_t i = 0; i < insz; ++i) {
    out += hex[(in[i] >> 4) & 0xF];
    out += hex[in[i] & 0xF];
  }

  return out;
}
