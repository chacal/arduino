#include <nrf_log.h>
#include <optional>
#include "settings.hpp"

#define JSON_PARSE_BUFFER_SIZE      2000

#include "ArduinoJson-v6.13.0.hpp"

using namespace ArduinoJson;

namespace settings {
  std::string m_instance = "D000";

  void updateFromCoapData(const coap_helpers::post_data &coap_data) {
    update(coap_data.data, coap_data.len);
  }

  void update(const uint8_t *data, uint32_t len) {
    NRF_LOG_DEBUG("Updating settings..")
    auto s = fromJson(data, len);
    if (!s) {
      return;
    }

    set_thread_settings(s);

    if (s->instance) {
      m_instance = *s->instance;
    }

    NRF_LOG_DEBUG("Updated settings: %s", get_as_json().c_str())
  }

  std::string get_as_json() {
    StaticJsonDocument<JSON_PARSE_BUFFER_SIZE> doc;
    write_thread_settings_to_json(doc);
    doc["instance"] = m_instance;
    return doc.as<std::string>();
  }

  std::optional<settings> fromJson(const uint8_t *jsonData, uint32_t len) {
    StaticJsonDocument<JSON_PARSE_BUFFER_SIZE> doc;
    DeserializationError                       error = deserializeJson(doc, jsonData);

    if (error) {
      NRF_LOG_ERROR("Parsing settings JSON failed! Error: %s", error.c_str());
      NRF_LOG_HEXDUMP_ERROR(jsonData, len)
      return std::nullopt;
    }

    settings s{base_settings_from_json(doc)};
    return s;
  }
}