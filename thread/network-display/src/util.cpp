#include "util.hpp"
#include "ArduinoJson-v5.13.4.hpp"
#include "vcc.hpp"

using namespace ArduinoJson;

static StaticJsonBuffer<200> jsonBuffer;


namespace util {

  std::string get_status_json() {
    JsonObject &root = jsonBuffer.createObject();
    root["vcc"] = vcc::measure();
    std::string output;
    root.printTo(output);
    return output;
  }

}
