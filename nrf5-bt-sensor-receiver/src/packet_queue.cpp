#include "packet_queue.hpp"
#include "util.hpp"
#include <app_util_platform.h>
#include <ble_gap.h>

std::optional<uint16_t> adv_packet::manufacturer_id() const {
  auto res = adv_field_data(BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA);

  if(res) {
    uint16_t manufacturer_id = (res.value()[0] << 8) | res.value()[1];  // First two bytes are the manufacturer ID
    return {manufacturer_id};
  } else {
    return std::nullopt;
  }
}

std::optional<std::vector<uint8_t>> adv_packet::adv_field_data(uint8_t adv_field_type) const {
  const uint8_t *p_data         = &data.payload[6];  // Skip first 6 bytes of adv report (they are a BLE MAC address)
  int           adv_data_length = data.payload_length - 6;

  int index = 0;
  while(index < adv_data_length - 1) {
    uint8_t field_length = p_data[index];
    uint8_t field_type   = p_data[index + 1];

    if(field_type == adv_field_type) {
      const uint8_t *field_data       = &p_data[index + 2];
      int           field_data_length = field_length - 1;
      return {{field_data, field_data + field_data_length}};
    }
    index += field_length + 1;
  }

  return std::nullopt;
}


packet_queue::packet_queue(size_t max_size) : max_size{max_size} {}

void packet_queue::push(const adv_packet &packet) {
  CRITICAL_REGION_ENTER();
  while(m_packets.size() >= max_size) {
    m_packets.pop();
  }
  m_packets.push(packet);
  CRITICAL_REGION_EXIT();
}

bool packet_queue::process_next_with(packet_processor processor) {
  CRITICAL_REGION_ENTER();
  if(!m_packets.empty()) {
    auto to_be_processed = m_packets.front();
    m_packets.pop();
    CRITICAL_REGION_EXIT();
    processor(to_be_processed);
  } else {
    CRITICAL_REGION_EXIT();
  }

  CRITICAL_REGION_ENTER();
  bool all_processed = m_packets.empty();
  CRITICAL_REGION_EXIT();

  return all_processed;
}
