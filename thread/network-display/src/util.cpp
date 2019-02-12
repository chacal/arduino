#include "util.hpp"
#include "ArduinoJson-v5.13.4.hpp"
#include "vcc.hpp"

using namespace ArduinoJson;

namespace util {

  std::string get_status_json() {
    StaticJsonBuffer<100> jsonBuffer;
    JsonObject            &root = jsonBuffer.createObject();
    root["vcc"] = vcc::measure();
    std::string output;
    root.printTo(output);
    return output;
  }

}
