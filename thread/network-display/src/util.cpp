#include "util.hpp"
#include "ArduinoJson-v5.13.4.hpp"
#include "vcc.hpp"

#define DEVICE_NAME         "D100"

using namespace ArduinoJson;

namespace util {

  std::string get_status_json() {
    StaticJsonBuffer<100> jsonBuffer;
    JsonObject            &root = jsonBuffer.createObject();
    root["vcc"]      = vcc::measure();
    root["instance"] = DEVICE_NAME;
    std::string output;
    root.printTo(output);
    return output;
  }

}
