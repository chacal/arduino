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
        thread::set_poll_period(milliseconds(doc["pollPeriod"]));
      } else {
        NRF_LOG_ERROR("Poll period must be an integer!");
      }
    }
  }

  std::string get_as_json() {
    StaticJsonDocument<512> doc;
    doc["txPower"]    = thread::get_tx_power();
    doc["pollPeriod"] = thread::get_poll_period().count();

    return doc.as<std::string>();
  }
}