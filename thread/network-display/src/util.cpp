#include <nrf_log.h>
#include <crc32.h>
#include "ArduinoJson-v6.13.0.hpp"
#include "vcc.hpp"
#include "thread.hpp"

#define DEVICE_NAME         "D108"

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
    StaticJsonDocument<512> doc;
    doc["vcc"]      = vcc::measure();
    doc["instance"] = DEVICE_NAME;

    auto       parent_info = thread::get_parent_info();
    JsonObject parent      = doc.createNestedObject("parent");

    parent["rloc16"]         = "0x" + n2hexstr(parent_info.rloc16);
    parent["linkQualityIn"]  = parent_info.link_quality_in;
    parent["linkQualityOut"] = parent_info.link_quality_out;
    parent["avgRssi"]        = parent_info.avg_rssi;
    parent["latestRssi"]     = parent_info.latest_rssi;

    return doc.as<std::string>();
  }

  void log_ipv6_address(const uint8_t *address) {
    char          buf[40];
    const uint8_t *m8 = address;
    sprintf(buf, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
            m8[0], m8[1], m8[2], m8[3], m8[4], m8[5], m8[6], m8[7], m8[8], m8[9], m8[10], m8[11], m8[12], m8[13], m8[14], m8[15]
    );
    NRF_LOG_INFO("%s", buf)
  }

  std::string get_device_id() {
    auto id = crc32_compute((const uint8_t *) NRF_FICR->DEVICEID, sizeof(NRF_FICR->DEVICEID), nullptr);
    return n2hexstr(id);
  }
}

