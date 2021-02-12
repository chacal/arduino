#pragma once

#include <cstdint>
#include <ArduinoJson-v6.13.0.hpp>

using namespace ArduinoJson;

class gpio_switch {
public:
  explicit gpio_switch(uint8_t pin_number, std::string json_field);

  void turn_on();

  void turn_off();

  void set_state_from_json(const JsonDocument &doc);

private:
  const uint8_t     pin_number;
  const std::string json_field;
};
