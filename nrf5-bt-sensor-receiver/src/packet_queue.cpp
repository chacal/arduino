#include "packet_queue.hpp"
#include <app_util_platform.h>

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
