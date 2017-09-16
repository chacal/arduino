#include <ble_advdata.h>
#include <app_error.h>
#include <ble/peer_manager/peer_manager.h>
#include <nrf_log.h>
#include "ble_adv_support.h"
#include "ble_data_service.h"

#define APP_ADV_INTERVAL                MSEC_TO_UNITS(50, UNIT_0_625_MS)
#define REMEMBERED_PEER_COUNT           BLE_GAP_WHITELIST_ADDR_MAX_COUNT

static ble_uuid_t           m_adv_uuids[] = {{DATA_SERVICE_SERVICE_UUID, DATA_SERVICE_UUID_TYPE}};
static ble_advdata_t        m_adv_packet_data;
static ble_advdata_t        m_scan_response_data;
static ble_gap_adv_params_t m_adv_params;


static void update_pm_whitelist_from_saved_peers() {
  // Fetch a list of peer IDs from Peer Manager and whitelist them.
  pm_peer_id_t peer_ids[REMEMBERED_PEER_COUNT] = {PM_PEER_ID_INVALID};
  uint32_t     n_peer_ids                      = 0;
  pm_peer_id_t peer_id                         = pm_next_peer_id_get(PM_PEER_ID_INVALID);

  while((peer_id != PM_PEER_ID_INVALID) && (n_peer_ids < REMEMBERED_PEER_COUNT)) {
    peer_ids[n_peer_ids++] = peer_id;
    peer_id = pm_next_peer_id_get(peer_id);
  }

  // Whitelist peers.
  APP_ERROR_CHECK(pm_whitelist_set(peer_ids, n_peer_ids));
}


static void adv_start_with_whitelist() {
  m_adv_packet_data.flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
  m_adv_params.fp         = BLE_GAP_ADV_FP_FILTER_BOTH;
  APP_ERROR_CHECK(ble_advdata_set(&m_adv_packet_data, &m_scan_response_data));


  static uint32_t            irk_cnt   = REMEMBERED_PEER_COUNT;
  static uint32_t            addr_cnt  = REMEMBERED_PEER_COUNT;
  static ble_gap_irk_t       irks[REMEMBERED_PEER_COUNT];
  static ble_gap_addr_t      addrs[REMEMBERED_PEER_COUNT];
  static ble_gap_addr_t      *m_p_whitelist_addrs[REMEMBERED_PEER_COUNT];
  static ble_gap_irk_t       *m_p_whitelist_irks[REMEMBERED_PEER_COUNT];
  static ble_gap_whitelist_t whitelist = {
      .pp_addrs = m_p_whitelist_addrs,
      .pp_irks  = m_p_whitelist_irks
  };

  update_pm_whitelist_from_saved_peers();
  APP_ERROR_CHECK(pm_whitelist_get(addrs, &addr_cnt, irks, &irk_cnt));

  whitelist.addr_count = addr_cnt;
  whitelist.irk_count  = irk_cnt;

  for(uint32_t i = 0; i < addr_cnt; i++) {
    whitelist.pp_addrs[i] = &addrs[i];
  }

  for(uint32_t i = 0; i < irk_cnt; i++) {
    whitelist.pp_irks[i] = &irks[i];
  }

  m_adv_params.p_whitelist = &whitelist;
  sd_ble_gap_adv_stop();
  APP_ERROR_CHECK(sd_ble_gap_adv_start(&m_adv_params));
  NRF_LOG_INFO("Advertising with whitelist\n")
}


static void adv_start_public() {
  m_adv_packet_data.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
  m_adv_params.fp         = BLE_GAP_ADV_FP_ANY;
  APP_ERROR_CHECK(ble_advdata_set(&m_adv_packet_data, &m_scan_response_data));

  m_adv_params.p_whitelist = NULL;
  sd_ble_gap_adv_stop();
  APP_ERROR_CHECK(sd_ble_gap_adv_start(&m_adv_params));
  NRF_LOG_INFO("Advertising public\n")
}


static void adv_stop() {
  NRF_LOG_INFO("Advertising stop\n")
  sd_ble_gap_adv_stop();
}


void ble_adv_init() {
  memset(&m_adv_packet_data, 0, sizeof(m_adv_packet_data));
  memset(&m_scan_response_data, 0, sizeof(m_scan_response_data));

  // Init advertisement packet data
  m_adv_packet_data.name_type          = BLE_ADVDATA_FULL_NAME;
  m_adv_packet_data.include_appearance = false;
  m_adv_packet_data.flags              = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;

  // Init scan response packet data
  m_scan_response_data.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
  m_scan_response_data.uuids_complete.p_uuids  = m_adv_uuids;

  APP_ERROR_CHECK(ble_advdata_set(&m_adv_packet_data, &m_scan_response_data));


  // Init advertisement configuration
  m_adv_params.type     = BLE_GAP_ADV_TYPE_ADV_IND;     // Connectable undirected.
  m_adv_params.fp       = BLE_GAP_ADV_FP_FILTER_BOTH;   // Filter both scan and connect requests with whitelist.
  m_adv_params.interval = APP_ADV_INTERVAL;
  m_adv_params.timeout  = 0;   // No timeout
}

void ble_adv_start() {
  if(pm_peer_count() == 0) {
    NRF_LOG_INFO("No saved peers. Not starting advertising.\n")
    adv_stop();
  } else {
    adv_start_with_whitelist();
  }
}

void ble_adv_stop() {
  adv_stop();
}

void ble_adv_discoverable_start() {
  adv_start_public();
}

