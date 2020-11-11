#include <nrf_log.h>
#include <optional>
#include "settings.hpp"

#define JSON_PARSE_BUFFER_SIZE      2000

#include "ArduinoJson-v6.13.0.hpp"
#include "thread.hpp"

using namespace ArduinoJson;

namespace settings {
  std::string m_instance;

  void updateFromCoapData(const coap_service::post_data &coap_data) {
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
  }

  std::string get_as_json() {
    StaticJsonDocument<JSON_PARSE_BUFFER_SIZE> doc;
    doc["instance"]              = m_instance;
    doc["txPower"]               = thread::get_tx_power();
    doc["pollPeriod"]            = thread::get_normal_poll_period().count();
    doc["increasedPollPeriod"]   = thread::get_increased_poll_period().count();
    doc["increasedPollDuration"] = thread::get_increased_poll_duration().count();
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

    return s;
  }
}