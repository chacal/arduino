#include <ble_advdata.h>
#include <peer_manager.h>
#include <nrf_log.h>
#include "ble_support_adv.hpp"
#include "ble_data_service.hpp"
#include "config.hpp"

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

  while ((peer_id != PM_PEER_ID_INVALID) && (n_peer_ids < REMEMBERED_PEER_COUNT)) {
    peer_ids[n_peer_ids++] = peer_id;
    peer_id = pm_next_peer_id_get(peer_id);
  }

  // Whitelist peers.
  APP_ERROR_CHECK(pm_whitelist_set(peer_ids, n_peer_ids));
}


namespace ble_support::adv {
  void init() {
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

  void connectable_start() {
    m_adv_packet_data.flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
    m_adv_params.fp         = BLE_GAP_ADV_FP_FILTER_BOTH;
    APP_ERROR_CHECK(ble_advdata_set(&m_adv_packet_data, &m_scan_response_data));
    update_pm_whitelist_from_saved_peers();

    sd_ble_gap_adv_stop();
    APP_ERROR_CHECK(sd_ble_gap_adv_start(&m_adv_params, BLE_CONN_CFG_TAG));
    NRF_LOG_INFO("Advertising with whitelist")
  }

  void discoverable_start() {
    m_adv_packet_data.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    m_adv_params.fp         = BLE_GAP_ADV_FP_ANY;
    APP_ERROR_CHECK(ble_advdata_set(&m_adv_packet_data, &m_scan_response_data));
    APP_ERROR_CHECK(pm_whitelist_set(NULL, 0));

    sd_ble_gap_adv_stop();
    APP_ERROR_CHECK(sd_ble_gap_adv_start(&m_adv_params, BLE_CONN_CFG_TAG));
    NRF_LOG_INFO("Advertising public")
  }

  void stop() {
    NRF_LOG_INFO("Advertising stop")
    sd_ble_gap_adv_stop();
  }
}
