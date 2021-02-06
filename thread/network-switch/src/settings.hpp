#pragma once

#include <chrono>
#include <memory>
#include "coap_helpers.hpp"
#include "base_settings.hpp"

#define MGMT_SERVER_PORT      5683

using namespace std::chrono;

namespace settings {
  extern std::string m_instance;

  struct settings : base_settings {
  };

  void updateFromCoapData(const coap_helpers::post_data &coap_data);

  void update(const uint8_t *data, uint32_t len);

  std::string get_as_json();

  std::optional<settings> fromJson(const uint8_t *jsonData, uint32_t len);
}