#pragma once

#include <queue>
#include <functional>

extern "C" {
#include "radio.h"
}

struct packet {
  nrf_packet_data data;
  int             rssi;
};

using packet_processor = std::function<void(const packet &)>;

class packet_queue {
public:
  explicit packet_queue(size_t max_size);
  void push(const packet &packet);
  bool process_next_with(packet_processor processor);

private:
  size_t             max_size;
  std::queue<packet> m_packets;
};
