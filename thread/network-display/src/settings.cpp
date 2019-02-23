#include <nrf_log.h>
#include "settings.hpp"

#define JSON_PARSE_BUFFER_SIZE      2000

#include "ArduinoJson-v5.13.4.hpp"
#include "thread.hpp"

using namespace ArduinoJson;

namespace settings {
  void update(const coap_service::post_data &coap_data) {
    NRF_LOG_DEBUG("Updating settings..")
    StaticJsonBuffer<JSON_PARSE_BUFFER_SIZE> doc;
    auto                                     &settings = doc.parseObject(coap_data.data);

    if (!settings.success()) {
      NRF_LOG_ERROR("Parsing settings JSON failed!");
      return;
    }

    if (settings.containsKey("txPower")) {
      if (settings["txPower"].is<int8_t>()) {
        thread::set_tx_power(settings["txPower"]);
      } else {
        NRF_LOG_ERROR("Tx power must be an integer!");
      }
    }
  }

  std::string get_as_json() {
    StaticJsonBuffer<300> jsonBuffer;
    JsonObject            &root = jsonBuffer.createObject();
    root["txPower"] = thread::get_tx_power();

    std::string output;
    root.printTo(output);
    return output;
  }
}