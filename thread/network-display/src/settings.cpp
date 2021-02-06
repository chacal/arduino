#include <nrf_log.h>
#include <optional>
#include "settings.hpp"

#define JSON_PARSE_BUFFER_SIZE      2000

#include "ArduinoJson-v6.13.0.hpp"
#include "thread.hpp"
#include <eink_display/good_display/good_display_1in54.hpp>
#include <eink_display/good_display/good_display_2in13.hpp>
#include <eink_display/good_display/good_display_2in9.hpp>
#include <eink_display/good_display/good_display_2in9_4gray.hpp>

using namespace ArduinoJson;

namespace settings {
  std::string                    m_instance     = "D000";
  std::optional<DisplayType>     m_display_type = std::nullopt;
  std::optional<HardwareVersion> m_hw_version   = std::nullopt;
  pin_config                     m_pin_config   = E73_BOARD_PINOUT;

  void updateFromCoapData(const coap_helpers::post_data &coap_data) {
    update(coap_data.data, coap_data.len);
  }

  void update(const uint8_t *data, uint32_t len) {
    NRF_LOG_DEBUG("Updating settings..")
    auto s = fromJson(data, len);
    if (!s) {
      return;
    }

    if (s->instance) {
      m_instance = *s->instance;
    }

    if (s->pollPeriod) {
      thread::set_tx_power(*s->txPower);
    }

    if (s->pollPeriod) {
      thread::set_normal_poll_period(milliseconds(*s->pollPeriod));
    }

    if (s->increasedPollPeriod) {
      thread::set_increased_poll_period(milliseconds(*s->increasedPollPeriod));
    }

    if (s->increasedPollDuration) {
      thread::set_increased_poll_duration(milliseconds(*s->increasedPollDuration));
    }

    if (s->displayType) {
      m_display_type = s->displayType;
    }

    if (s->hwVersion) {
      m_hw_version = s->hwVersion;
      if (m_hw_version == E73) {
        m_pin_config = E73_BOARD_PINOUT;
      } else {
        m_pin_config = MS88SF2_BOARD_PINOUT;
      }
    }

    NRF_LOG_DEBUG("Updated settings: %s", get_as_json().c_str())
  }

  std::string get_as_json() {
    StaticJsonDocument<JSON_PARSE_BUFFER_SIZE> doc;
    doc["instance"]              = m_instance;
    doc["txPower"]               = thread::get_tx_power();
    doc["pollPeriod"]            = thread::get_normal_poll_period().count();
    doc["increasedPollPeriod"]   = thread::get_increased_poll_period().count();
    doc["increasedPollDuration"] = thread::get_increased_poll_duration().count();
    doc["displayType"]           = toString(m_display_type);
    doc["hwVersion"]             = toString(m_hw_version);
    return doc.as<std::string>();
  }

  std::optional<settings> fromJson(const uint8_t *jsonData, uint32_t len) {
    settings                                   s{};
    StaticJsonDocument<JSON_PARSE_BUFFER_SIZE> doc;
    DeserializationError                       error = deserializeJson(doc, jsonData);

    if (error) {
      NRF_LOG_ERROR("Parsing settings JSON failed! Error: %s", error.c_str());
      NRF_LOG_HEXDUMP_ERROR(jsonData, len)
      return std::nullopt;
    }

    if (doc.containsKey("instance")) {
      if (doc["instance"].is<char *>()) {
        s.instance = doc["instance"].as<char *>();
      } else {
        NRF_LOG_ERROR("Instance must be a string!");
      }
    }

    if (doc.containsKey("txPower")) {
      if (doc["txPower"].is<int8_t>()) {
        s.txPower = doc["txPower"];
      } else {
        NRF_LOG_ERROR("Tx power must be an integer!");
      }
    }

    if (doc.containsKey("pollPeriod")) {
      if (doc["pollPeriod"].is<uint32_t>()) {
        s.pollPeriod = milliseconds(doc["pollPeriod"]);
      } else {
        NRF_LOG_ERROR("Poll period must be an integer!");
      }
    }

    if (doc.containsKey("increasedPollPeriod")) {
      if (doc["increasedPollPeriod"].is<uint32_t>()) {
        s.increasedPollPeriod = milliseconds(doc["increasedPollPeriod"]);
      } else {
        NRF_LOG_ERROR("Increased poll period must be an integer!");
      }
    }

    if (doc.containsKey("increasedPollDuration")) {
      if (doc["increasedPollDuration"].is<uint32_t>()) {
        s.increasedPollDuration = milliseconds(doc["increasedPollDuration"]);
      } else {
        NRF_LOG_ERROR("Increased poll duration must be an integer!");
      }
    }

    if (doc.containsKey("displayType")) {
      if (doc["displayType"].is<char *>()) {
        auto displayType = displayTypeFromString(doc["displayType"]);
        if (displayType) {
          s.displayType = displayType;
        } else {
          NRF_LOG_ERROR("Unknown display type! %s", doc["displayType"]);
        }
      } else {
        NRF_LOG_ERROR("Display type must be a string!");
      }
    }

    if (doc.containsKey("hwVersion")) {
      if (doc["hwVersion"].is<char *>()) {
        auto hwVersion = hwVersionFromString(doc["hwVersion"]);
        if (hwVersion) {
          s.hwVersion = hwVersion;
        } else {
          NRF_LOG_ERROR("Unknown hardware version! %s", doc["hwVersion"]);
        }
      } else {
        NRF_LOG_ERROR("Hardware version must be a string!");
      }
    }

    return s;
  }

  std::unique_ptr<display> createDisplay(DisplayType displayType) {
    switch (displayType) {
      case GOOD_DISPLAY_1_54IN:
        return std::make_unique<good_display_1in54>();
      case GOOD_DISPLAY_2_13IN:
        return std::make_unique<good_display_2in13>();
      case GOOD_DISPLAY_2_9IN:
        return std::make_unique<good_display_2in9>();
      case GOOD_DISPLAY_2_9IN_4GRAY:
        return std::make_unique<good_display_2in9_4gray>();
      default:  // Just to make compiler happy..
        return std::make_unique<good_display_2in13>();
    }
  }

  std::string toString(std::optional<DisplayType> displayType) {
    if (displayType) {
      switch (*displayType) {
        case GOOD_DISPLAY_1_54IN:
          return "GOOD_DISPLAY_1_54IN";
        case GOOD_DISPLAY_2_13IN:
          return "GOOD_DISPLAY_2_13IN";
        case GOOD_DISPLAY_2_9IN:
          return "GOOD_DISPLAY_2_9IN";
        case GOOD_DISPLAY_2_9IN_4GRAY:
          return "GOOD_DISPLAY_2_9IN_4GRAY";
        default:
          return "";
      }
    } else {
      return "";
    }
  }

  std::optional<DisplayType> displayTypeFromString(const std::string &str) {
    if (str == "GOOD_DISPLAY_1_54IN") {
      return GOOD_DISPLAY_1_54IN;
    } else if (str == "GOOD_DISPLAY_2_13IN") {
      return GOOD_DISPLAY_2_13IN;
    } else if (str == "GOOD_DISPLAY_2_9IN") {
      return GOOD_DISPLAY_2_9IN;
    } else if (str == "GOOD_DISPLAY_2_9IN_4GRAY") {
      return GOOD_DISPLAY_2_9IN_4GRAY;
    } else {
      return std::nullopt;
    }
  }

  std::string toString(std::optional<HardwareVersion> hwVersion) {
    if (hwVersion) {
      switch (*hwVersion) {
        case E73:
          return "E73";
        case MS88SF2_V1_0:
          return "MS88SF2_V1_0";
        default:
          return "";
      }
    } else {
      return "";
    }
  }

  std::optional<HardwareVersion> hwVersionFromString(const std::string &str) {
    if (str == "E73") {
      return E73;
    } else if (str == "MS88SF2_V1_0") {
      return MS88SF2_V1_0;
    } else {
      return std::nullopt;
    }
  }
}