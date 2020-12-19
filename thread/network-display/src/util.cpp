#include <nrf_log.h>
#include <crc32.h>
#include "ArduinoJson-v6.13.0.hpp"
#include "vcc.hpp"
#include "thread.hpp"
#include "settings.hpp"
#include "util.hpp"


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

  std::string get_state_json() {
    StaticJsonDocument<1024> doc;
    doc["vcc"]      = vcc::measure();
    doc["instance"] = settings::m_instance;

    auto       parent_info = thread::get_parent_info();
    JsonObject parent      = doc.createNestedObject("parent");

    parent["rloc16"]         = "0x" + n2hexstr(parent_info.rloc16);
    parent["linkQualityIn"]  = parent_info.link_quality_in;
    parent["linkQualityOut"] = parent_info.link_quality_out;
    parent["avgRssi"]        = parent_info.avg_rssi;
    parent["latestRssi"]     = parent_info.latest_rssi;

    JsonArray addresses = doc.createNestedArray("addresses");

    for (auto addr = otIp6GetUnicastAddresses(thread::get_instance()); addr; addr = addr->mNext) {
      addresses.add(util::ipv6_to_str(*addr));
    }

    return doc.as<std::string>();
  }

  std::string ipv6_to_str(const otNetifAddress &address) {
    char          buf[40];
    const uint8_t *m8 = address.mAddress.mFields.m8;
    sprintf(buf, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
            m8[0], m8[1], m8[2], m8[3], m8[4], m8[5], m8[6], m8[7], m8[8], m8[9], m8[10], m8[11], m8[12], m8[13], m8[14], m8[15]
    );
    return buf;
  }

  void log_ipv6_address(const otNetifAddress &address) {
    NRF_LOG_INFO("%s", ipv6_to_str(address).c_str())
  }

  std::string get_device_id() {
    auto id = crc32_compute((const uint8_t *) NRF_FICR->DEVICEID, sizeof(NRF_FICR->DEVICEID), nullptr);
    return n2hexstr(id);
  }

  otNetifAddress create_ip6_addr_for_prefix(const otBorderRouterConfig &conf) {
    otNetifAddress addr{0};
    auto           prefix = conf.mPrefix;
    addr.mPrefixLength = prefix.mLength;
    addr.mPreferred    = conf.mPreferred;
    addr.mValid        = true;
    memcpy(&addr.mAddress, &prefix.mPrefix, OT_IP6_PREFIX_SIZE);
    memcpy(&addr.mAddress.mFields.m8[OT_IP6_PREFIX_SIZE], (const uint8_t *) NRF_FICR->DEVICEID, OT_IP6_ADDRESS_SIZE - OT_IP6_PREFIX_SIZE);
    return addr;
  }

  bool has_addr_for_prefix(otInstance *instance, const otIp6Prefix &prefix) {
    bool found = false;

    for (auto *addr = otIp6GetUnicastAddresses(instance); addr; addr = addr->mNext) {
      if (addr->mPrefixLength == prefix.mLength && otIp6PrefixMatch(&addr->mAddress, &prefix.mPrefix) >= prefix.mLength) {
        found = true;
        break;
      }
    }

    return found;
  }

  void print_addresses(otInstance *instance) {
    NRF_LOG_INFO("All IP6 addresses:")
    for (auto addr = otIp6GetUnicastAddresses(instance); addr; addr = addr->mNext) {
      util::log_ipv6_address(*addr);
    }
  }
}
