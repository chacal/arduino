#pragma once

#include <chrono>

#include "coap_service.hpp"

using namespace std::chrono;

namespace settings {
  extern std::string m_instance;

  struct settings {
    std::optional<std::string>  instance;
    std::optional<int8_t>       txPower;
    std::optional<milliseconds> pollPeriod;
    std::optional<milliseconds> increasedPollPeriod;
    std::optional<milliseconds> increasedPollDuration;
  };

  void updateFromCoapData(const coap_service::post_data &coap_data);

  void update(const uint8_t *data, uint32_t len);

  std::string get_as_json();

  std::optional<settings> fromJson(const uint8_t *jsonData, uint32_t len);
}