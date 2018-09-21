#include <ble_advdata.h>
#include <peer_manager.h>
#include <nrf_log.h>
#include "ble_support_adv.hpp"
#include "ble_data_service.hpp"

#define APP_ADV_INTERVAL                MSEC_TO_UNITS(50, UNIT_0_625_MS)
#define REMEMBERED_PEER_COUNT           BLE_GAP_WHITELIST_ADDR_MAX_COUNT

static ble_uuid_t           m_adv_uuids[] = {{DATA_SERVICE_SERVICE_UUID, DATA_SERVICE_UUID_TYPE}};
static ble_advdata_t        m_adv_packet_data;
static ble_advdata_t        m_scan_response_data;
static ble_gap_adv_params_t m_adv_params;


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
}
