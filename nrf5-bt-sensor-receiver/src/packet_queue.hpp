#pragma once

#include <queue>
#include <functional>
#include <optional>

#include "radio.hpp"

struct adv_packet {
  nrf_packet_data data;
  int             rssi;
  std::optional<uint16_t> manufacturer_id() const;
  std::optional<std::vector<uint8_t>> adv_field_data(uint8_t adv_field_type) const;
};

using packet_processor = std::function<void(const adv_packet &)>;

class packet_queue {
public:
  explicit packet_queue(size_t max_size);
  void push(const adv_packet &packet);
  bool process_next_with(packet_processor processor);

private:
  size_t             max_size;
  std::queue<adv_packet> m_packets;
};
