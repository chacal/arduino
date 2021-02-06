#pragma once

#include "ArduinoJson-v6.13.0.hpp"

using namespace std::chrono;
using namespace ArduinoJson;

namespace settings {
  struct base_settings {
    std::optional<std::string>  instance;
    std::optional<int8_t>       txPower;
    std::optional<milliseconds> pollPeriod;
    std::optional<milliseconds> increasedPollPeriod;
    std::optional<milliseconds> increasedPollDuration;
  };

  base_settings base_settings_from_json(const JsonDocument &doc);

  void write_thread_settings_to_json(JsonDocument &doc);
  void set_thread_settings(const std::optional<base_settings> &s);
}