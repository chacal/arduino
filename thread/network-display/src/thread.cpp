#include "thread_credentials.h"
#include <openthread/ip6.h>
#include <openthread/child_supervision.h>
#include <nrf_log_ctrl.h>
#include <nrf_log.h>
#include <nrf_log_default_backends.h>
#include <app_timer.h>

#include "thread.hpp"

extern "C" {
#include <thread_utils.h>
}

#define LOG_IP6_ADDRESSES                   true
#define TX_POWER                               0  // dBm
#define SED_NORMAL_POLL_PERIOD_MS           5000
#define SED_INCREASED_POLL_PERIOD_MS         400
#define SED_INCREASED_POLL_DURATION_MS     15000
#define CHILD_TIMEOUT_S                       60

namespace thread {

  static thread_role_handler_t m_thread_role_handler;
  static milliseconds          m_normal_poll_period          = milliseconds(SED_NORMAL_POLL_PERIOD_MS);
  static milliseconds          m_increased_poll_period       = milliseconds(SED_INCREASED_POLL_PERIOD_MS);
  static milliseconds          m_increased_poll_duration     = milliseconds(SED_INCREASED_POLL_DURATION_MS);
  static bool                  m_using_increased_poll_period = false;
  APP_TIMER_DEF(m_increased_poll_rate_timer);

  static void print_ipv6_address(const otNetifAddress *address) {
    char          buf[40];
    const uint8_t *m8 = address->mAddress.mFields.m8;
    sprintf(buf, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
            m8[0], m8[1], m8[2], m8[3], m8[4], m8[5], m8[6], m8[7], m8[8], m8[9], m8[10], m8[11], m8[12], m8[13], m8[14], m8[15]
    );
    NRF_LOG_INFO("%s", buf)
  }

  static void print_addresses() {
    if (LOG_IP6_ADDRESSES) {
      const otNetifAddress *addr = (otIp6GetUnicastAddresses(thread_ot_instance_get()));
      for (; addr; addr = addr->mNext) {
        print_ipv6_address(addr);
      }
    }
  }

  static void thread_state_changed_callback(uint32_t flags, void *p_context) {
    otDeviceRole role = otThreadGetDeviceRole(static_cast<otInstance *>(p_context));
    NRF_LOG_INFO("State changed! Flags: 0x%08x Current role: %d", flags, role);
    if ((flags & OT_CHANGED_THREAD_NETDATA) != 0) {
      print_addresses();
    }
    m_thread_role_handler(role);
  }

  static void on_poll_rate_timer(void *p_context) {
    end_increased_poll_rate();
  }

  otInstance *initialize(const thread_role_handler_t &role_handler) {
    m_thread_role_handler = role_handler;

    otExtendedPanId xPanId    = {.m8 = THREAD_XPANID};
    otMasterKey     masterKey = {.m8 = THREAD_MASTERKEY};

    thread_configuration_t thread_configuration;
    thread_init(&thread_configuration);

    otInstance *ot = thread_ot_instance_get();
    ASSERT_OT(otLinkSetPanId(ot, THREAD_PANID));
    ASSERT_OT(otThreadSetExtendedPanId(ot, &xPanId));
    ASSERT_OT(otThreadSetMasterKey(ot, &masterKey));
    ASSERT_OT(otThreadSetNetworkName(ot, THREAD_NETWORK_NAME));
    ASSERT_OT(otLinkSetChannel(ot, THREAD_CHANNEL));
    ASSERT_OT(otIp6SetEnabled(ot, true));
    ASSERT_OT(otPlatRadioSetTransmitPower(ot, TX_POWER));

    otLinkModeConfig mode;
    mode.mRxOnWhenIdle       = false;
    mode.mSecureDataRequests = true;
    mode.mNetworkData        = false;
    mode.mDeviceType         = false;
    ASSERT_OT(otThreadSetLinkMode(ot, mode));
    ASSERT_OT(otIp6SetEnabled(ot, true));

    ASSERT_OT(otThreadSetEnabled(ot, true));

    otLinkSetPollPeriod(ot, m_normal_poll_period.count());
    otThreadSetChildTimeout(ot, CHILD_TIMEOUT_S);
    otIp6SetSlaacEnabled(ot, true);

    NRF_LOG_INFO("802.15.4 Channel: %d", otLinkGetChannel(ot));
    NRF_LOG_INFO("802.15.4 PAN ID:  0x%04x", otLinkGetPanId(ot));
    NRF_LOG_INFO("802.15.4 Extended PAN ID:");
    NRF_LOG_HEXDUMP_INFO(otThreadGetExtendedPanId(ot)->m8, OT_EXT_PAN_ID_SIZE);
    NRF_LOG_INFO("802.15.4 Network/Master Key:");
    NRF_LOG_HEXDUMP_INFO(otThreadGetMasterKey(ot)->m8, OT_MASTER_KEY_SIZE);
    NRF_LOG_INFO("802.15.4 Network Name: %s", otThreadGetNetworkName(ot));
    NRF_LOG_INFO("rx-on-when-idle:  %s", otThreadGetLinkMode(ot).mRxOnWhenIdle ? "enabled" : "disabled");
    NRF_LOG_INFO("Is FFD:  %s", otThreadGetLinkMode(ot).mDeviceType ? "yes" : "no");
    NRF_LOG_INFO("Requires full network data:  %s", otThreadGetLinkMode(ot).mNetworkData ? "yes" : "no");
    int8_t tx_power = 0;
    ASSERT_OT(otPlatRadioGetTransmitPower(ot, &tx_power))
    NRF_LOG_INFO("TX Power: %d dBm", tx_power);
    NRF_LOG_INFO("SED Poll Period: %d ms", otLinkGetPollPeriod(ot));

    NRF_LOG_INFO("Child timeout: %d s", otThreadGetChildTimeout(ot));
    NRF_LOG_INFO("SLAAC enabled: %d", otIp6IsSlaacEnabled(ot));

    //thread_cli_init();
    thread_state_changed_callback_set(thread_state_changed_callback);

    APP_ERROR_CHECK(app_timer_create(&m_increased_poll_rate_timer, APP_TIMER_MODE_SINGLE_SHOT, on_poll_rate_timer));
    return ot;
  }

  parent_info get_parent_info() {
    otInstance *ot = thread_ot_instance_get();
    ASSERT(ot != nullptr);

    int8_t avg_rssi, latest_rssi;
    otThreadGetParentAverageRssi(ot, &avg_rssi);
    otThreadGetParentLastRssi(ot, &latest_rssi);

    otRouterInfo parentInfo;
    otThreadGetParentInfo(ot, &parentInfo);

    return {
      parentInfo.mRloc16,
      parentInfo.mLinkQualityIn,
      parentInfo.mLinkQualityOut,
      avg_rssi,
      latest_rssi
    };
  }

  void set_tx_power(int8_t power) {
    otInstance *ot = thread_ot_instance_get();
    ASSERT(ot != nullptr);
    if (power >= -20 && power <= 8) {
      NRF_LOG_DEBUG("Setting TX power to %ddBm", power);
      ASSERT_OT(otPlatRadioSetTransmitPower(ot, power));
    } else {
      NRF_LOG_ERROR("Invalid TX power! %ddBm", power)
    }
  }

  int8_t get_tx_power() {
    otInstance *ot = thread_ot_instance_get();
    ASSERT(ot != nullptr);
    int8_t power;
    ASSERT_OT(otPlatRadioGetTransmitPower(ot, &power));
    return power;
  }

  void set_ot_poll_period(milliseconds poll_period) {
    otInstance *ot = thread_ot_instance_get();
    ASSERT(ot != nullptr);
    NRF_LOG_DEBUG("Setting poll period to %dms", poll_period.count());
    otLinkSetPollPeriod(ot, static_cast<uint32_t>(poll_period.count()));
  }

  void set_normal_poll_period(milliseconds poll_period) {
    m_normal_poll_period = poll_period;
    if (!m_using_increased_poll_period) {
      set_ot_poll_period(m_normal_poll_period);
    }
  }

  milliseconds get_normal_poll_period() {
    return m_normal_poll_period;
  }

  void set_increased_poll_period(milliseconds poll_period) {
    m_increased_poll_period = poll_period;
    if (m_using_increased_poll_period) {
      set_ot_poll_period(m_increased_poll_period);
    }
  }

  milliseconds get_increased_poll_period() {
    return m_increased_poll_period;
  }

  void set_increased_poll_duration(milliseconds duration) {
    m_increased_poll_duration = duration;
  }

  milliseconds get_increased_poll_duration() {
    return m_increased_poll_duration;
  }

  void begin_increased_poll_rate() {
    m_using_increased_poll_period = true;
    set_ot_poll_period(get_increased_poll_period());
    app_timer_stop(m_increased_poll_rate_timer);
    app_timer_start(m_increased_poll_rate_timer, APP_TIMER_TICKS(m_increased_poll_duration.count()), nullptr);
  }

  void end_increased_poll_rate() {
    m_using_increased_poll_period = false;
    app_timer_stop(m_increased_poll_rate_timer);
    set_ot_poll_period(get_normal_poll_period());
  }

  void run() {
    thread_process();
    thread_sleep();
  }
}