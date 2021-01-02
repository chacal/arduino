#pragma once

#include <chrono>
#include <memory>
#include <display.hpp>
#include "coap_service.hpp"

#define MGMT_SERVER_PORT      5683

using namespace std::chrono;

namespace settings {
  enum DisplayType {
    GOOD_DISPLAY_1_54IN,
    GOOD_DISPLAY_2_13IN,
    GOOD_DISPLAY_2_9IN,
    GOOD_DISPLAY_2_9IN_4GRAY
  };

  extern std::string                m_instance;
  extern std::string                m_mgmt_server;
  extern std::optional<DisplayType> m_display_type;

  struct settings {
    std::optional<std::string>  instance;
    std::optional<int8_t>       txPower;
    std::optional<milliseconds> pollPeriod;
    std::optional<milliseconds> increasedPollPeriod;
    std::optional<milliseconds> increasedPollDuration;
    std::optional<DisplayType>  displayType;
  };

  void updateFromCoapData(const coap_service::post_data &coap_data);

  void update(const uint8_t *data, uint32_t len);

  std::string get_as_json();

  std::optional<settings> fromJson(const uint8_t *jsonData, uint32_t len);

  std::unique_ptr<display> createDisplay(DisplayType displayType);

  std::string toString(std::optional<DisplayType> displayType);

  std::optional<DisplayType> displayTypeFromString(const std::string &str);
}