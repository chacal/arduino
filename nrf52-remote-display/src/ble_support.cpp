#include <ble.h>
#include <nrf_sdh.h>
#include <cstring>
#include <ble_conn_params.h>
#include <app_timer.h>
#include <peer_manager.h>
#include <ecc.h>
#include <nrf_log.h>
#include "ble_support.hpp"
#include "config.hpp"


#define DEVICE_NAME               "MarineDisplay"
#define TX_POWER_LEVEL            4
#define APP_BLE_OBSERVER_PRIO     2

#define MIN_CONN_INTERVAL         MSEC_TO_UNITS(20, UNIT_1_25_MS)
#define MAX_CONN_INTERVAL         MSEC_TO_UNITS(20, UNIT_1_25_MS)
#define SLAVE_LATENCY             12
#define CONN_SUP_TIMEOUT          MSEC_TO_UNITS(4000, UNIT_10_MS)

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(1000)                    /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000)                    /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

typedef struct { uint8_t sk[32]; }       ble_gap_lesc_p256_sk_t;
__ALIGN(4) static ble_gap_lesc_p256_sk_t m_lesc_sk;    /* LESC private key */
__ALIGN(4) static ble_gap_lesc_p256_pk_t m_lesc_pk;    /* LESC public key */


namespace ble_support {

  static ble_evt_handler_t m_evt_handler;

  static void on_ble_event(const ble_evt_t *p_ble_evt, void *ctx) {
    m_evt_handler(p_ble_evt);
  }

  static void ble_stack_init(const ble_evt_handler_t &ble_evt_handler) {
    m_evt_handler = ble_evt_handler;

    APP_ERROR_CHECK(nrf_sdh_enable_request());

    // Configure the BLE stack using the default settings in sdk_config.h
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    APP_ERROR_CHECK(nrf_sdh_ble_default_cfg_set(BLE_CONN_CFG_TAG, &ram_start));

    // Enable BLE stack.
    APP_ERROR_CHECK(nrf_sdh_ble_enable(&ram_start));

    sd_ble_gap_tx_power_set(TX_POWER_LEVEL);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, on_ble_event, NULL);
  }

  static void gap_params_init() {
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    APP_ERROR_CHECK(sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *) DEVICE_NAME, strlen(DEVICE_NAME)));

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    APP_ERROR_CHECK(sd_ble_gap_ppcp_set(&gap_conn_params));
  }

  static void conn_params_init() {
    ble_conn_params_init_t cp_init;
    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.error_handler                  = [](uint32_t nrf_error) { APP_ERROR_HANDLER(nrf_error); };
    cp_init.evt_handler                    = [](ble_conn_params_evt_t * p_evt) { NRF_LOG_INFO("Conn params evt: %d", p_evt->evt_type); };

    APP_ERROR_CHECK(ble_conn_params_init(&cp_init));
  }

  static void pairing_init() {
    ecc_init(true);

    APP_ERROR_CHECK(pm_init());
    APP_ERROR_CHECK(ecc_p256_keypair_gen(m_lesc_sk.sk, m_lesc_pk.pk));
    APP_ERROR_CHECK(pm_lesc_public_key_set(&m_lesc_pk));

    ble_gap_sec_params_t sec_param;
    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = 1;
    sec_param.mitm           = 1;
    sec_param.lesc           = 1;
    sec_param.io_caps        = BLE_GAP_IO_CAPS_DISPLAY_ONLY;
    sec_param.min_key_size   = 7;
    sec_param.max_key_size   = 16;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    APP_ERROR_CHECK(pm_sec_params_set(&sec_param));
    //APP_ERROR_CHECK(pm_register(pm_evt_handler));
  }


  void init(const ble_evt_handler_t &ble_evt_handler) {
    ble_stack_init(ble_evt_handler);
    gap_params_init();
    conn_params_init();
    pairing_init();
  }
}
