#include <nrf_log.h>
#include "settings.hpp"

#define JSON_PARSE_BUFFER_SIZE      2000

#include "ArduinoJson-v6.13.0.hpp"
#include "thread.hpp"

using namespace ArduinoJson;

namespace settings {
  void update(const coap_service::post_data &coap_data) {
    NRF_LOG_DEBUG("Updating settings..")
    StaticJsonDocument<JSON_PARSE_BUFFER_SIZE> doc;
    DeserializationError                       error = deserializeJson(doc, coap_data.data);

    if (error) {
      NRF_LOG_ERROR("Parsing settings JSON failed!");
      return;
    }

    if (doc.containsKey("txPower")) {
      if (doc["txPower"].is<int8_t>()) {
        thread::set_tx_power(doc["txPower"]);
      } else {
        NRF_LOG_ERROR("Tx power must be an integer!");
      }
    }

    if (doc.containsKey("pollPeriod")) {
      if (doc["pollPeriod"].is<uint32_t>()) {
        thread::set_normal_poll_period(milliseconds(doc["pollPeriod"]));
      } else {
        NRF_LOG_ERROR("Poll period must be an integer!");
      }
    }

    if (doc.containsKey("increasedPollPeriod")) {
      if (doc["increasedPollPeriod"].is<uint32_t>()) {
        thread::set_increased_poll_period(milliseconds(doc["increasedPollPeriod"]));
      } else {
        NRF_LOG_ERROR("Increased poll period must be an integer!");
      }
    }

    if (doc.containsKey("increasedPollDuration")) {
      if (doc["increasedPollDuration"].is<uint32_t>()) {
        thread::set_increased_poll_duration(milliseconds(doc["increasedPollDuration"]));
      } else {
        NRF_LOG_ERROR("Increased poll duration must be an integer!");
      }
    }
  }

  std::string get_as_json() {
    StaticJsonDocument<512> doc;
    doc["txPower"]    = thread::get_tx_power();
    doc["pollPeriod"] = thread::get_normal_poll_period().count();
    doc["increasedPollPeriod"] = thread::get_increased_poll_period().count();
    doc["increasedPollDuration"] = thread::get_increased_poll_duration().count();

    return doc.as<std::string>();
  }
}