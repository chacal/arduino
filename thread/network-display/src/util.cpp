#include "util.hpp"
#include "ArduinoJson-v5.13.4.hpp"
#include "vcc.hpp"
#include "thread.hpp"

#define DEVICE_NAME         "D100"

using namespace ArduinoJson;

namespace util {

  template<typename I>
  static std::string n2hexstr(I w, size_t hex_len = sizeof(I) << 1) {
    static const char *digits = "0123456789ABCDEF";
    std::string       rc(hex_len, '0');

    for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
      rc[i] = digits[(w >> j) & 0x0f];

    return rc;
  }

  std::string get_status_json() {
    StaticJsonBuffer<300> jsonBuffer;
    JsonObject            &root = jsonBuffer.createObject();
    root["vcc"]      = vcc::measure();
    root["instance"] = DEVICE_NAME;

    auto       parent_info = thread::get_parent_info();
    JsonObject &parent     = root.createNestedObject("parent");
    parent["rloc16"]      = "0x" + n2hexstr(parent_info.rloc16);
    parent["linkQualityIn"]   = parent_info.link_quality_in;
    parent["linkQualityOut"]  = parent_info.link_quality_out;
    parent["avgRssi"]    = parent_info.avg_rssi;
    parent["latestRssi"] = parent_info.latest_rssi;

    std::string output;
    root.printTo(output);
    return output;
  }

}

