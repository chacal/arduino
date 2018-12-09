#include "thread_credentials.h"
#include <openthread/thread.h>
#include <thread_utils.h>
#include <openthread/ip6.h>
#include <nrf_log_ctrl.h>
#include <nrf_log.h>
#include <nrf_log_default_backends.h>

#include "thread.h"
#include "config.h"

static otNetifAddress                       ip6_addresses[MAX_IP6_ADDRESS_COUNT];
static uint8_t                              ip6_identity[24];
static otSemanticallyOpaqueIidGeneratorData data = {
    .mInterfaceId = (unsigned char *) "wpan0",
    .mInterfaceIdLength = 6,
    .mDadCounter = 0,
    .mNetworkId = NULL,
    .mNetworkIdLength = 0,
    .mSecretKey = ip6_identity,
    .mSecretKeyLength = sizeof(ip6_identity)
};

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
  NRF_LOG_INFO("State changed! Flags: 0x%08x Current role: %d", flags, otThreadGetDeviceRole(p_context));
  if ((flags & OT_CHANGED_THREAD_NETDATA) != 0) {
    otIp6SlaacUpdate(thread_ot_instance_get(), ip6_addresses, MAX_IP6_ADDRESS_COUNT, otIp6CreateSemanticallyOpaqueIid, &data);
    print_addresses();
  }
}

static void initialize_ip6_identity() {
  uint8_t reg_size = sizeof(NRF_FICR->DEVICEID[0]);
  memcpy(ip6_identity, (void *) &NRF_FICR->DEVICEID[0], reg_size);
  memcpy(ip6_identity + reg_size, (void *) &NRF_FICR->DEVICEID[1], reg_size);
  memcpy(ip6_identity + 2 * reg_size, (void *) &NRF_FICR->IR[0], reg_size);
  memcpy(ip6_identity + 3 * reg_size, (void *) &NRF_FICR->IR[1], reg_size);
  memcpy(ip6_identity + 4 * reg_size, (void *) &NRF_FICR->IR[2], reg_size);
  memcpy(ip6_identity + 5 * reg_size, (void *) &NRF_FICR->IR[3], reg_size);
}

void thread_initialize() {
  initialize_ip6_identity();

  otExtendedPanId        xPanId               = {.m8 = THREAD_XPANID};
  otMasterKey            masterKey            = {.m8 = THREAD_MASTERKEY};
  thread_configuration_t thread_configuration =
                             {
                                 .role                  = RX_ON_WHEN_IDLE,
                                 .autocommissioning     = false,
                                 .autostart_disable     = true,
                             };

  thread_init(&thread_configuration);

  otInstance *ot = thread_ot_instance_get();
  ASSERT_OT(otLinkSetPanId(ot, THREAD_PANID));
  ASSERT_OT(otThreadSetExtendedPanId(ot, &xPanId));
  ASSERT_OT(otThreadSetMasterKey(ot, &masterKey));
  ASSERT_OT(otThreadSetNetworkName(ot, THREAD_NETWORK_NAME));
  ASSERT_OT(otLinkSetChannel(ot, THREAD_CHANNEL));
  ASSERT_OT(otIp6SetEnabled(ot, true));
  ASSERT_OT(otPlatRadioSetTransmitPower(ot, TX_POWER));
  ASSERT_OT(otThreadSetEnabled(ot, true));

  NRF_LOG_INFO("802.15.4 Channel: %d", otLinkGetChannel(ot));
  NRF_LOG_INFO("802.15.4 PAN ID:  0x%04x", otLinkGetPanId(ot));
  NRF_LOG_INFO("802.15.4 Extended PAN ID:");
  NRF_LOG_HEXDUMP_INFO(otThreadGetExtendedPanId(ot)->m8, OT_EXT_PAN_ID_SIZE);
  NRF_LOG_INFO("802.15.4 Network/Master Key:");
  NRF_LOG_HEXDUMP_INFO(otThreadGetMasterKey(ot)->m8, OT_MASTER_KEY_SIZE);
  NRF_LOG_INFO("rx-on-when-idle:  %s", otThreadGetLinkMode(ot).mRxOnWhenIdle ? "enabled" : "disabled");
  int8_t tx_power = 0;
  ASSERT_OT(otPlatRadioGetTransmitPower(ot, &tx_power))
  NRF_LOG_INFO("TX Power: %d dBm", tx_power);

  thread_cli_init();
  thread_state_changed_callback_set(thread_state_changed_callback);
}
