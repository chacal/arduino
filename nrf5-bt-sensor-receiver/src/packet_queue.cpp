#include "packet_queue.hpp"
#include "util.hpp"
#include <app_util_platform.h>
#include <ble_gap.h>

std::optional<uint16_t> packet::manufacturer_id() const {
  auto res = Util::getAdvPacketField(BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, &data);

  if(res) {
    uint16_t manufacturer_id = (res.value()[0] << 8) | res.value()[1];  // First two bytes are the manufacturer ID
    return {manufacturer_id};
  } else {
    return std::nullopt;
  }
}



packet_queue::packet_queue(size_t max_size) : max_size{max_size} {}

void packet_queue::push(const packet &packet) {
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
