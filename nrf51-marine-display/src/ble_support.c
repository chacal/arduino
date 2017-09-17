#include <stdint.h>
#include <ble.h>
#include <ble_hci.h>
#include <app_error.h>
#include <bsp.h>
#include <memory.h>
#include <ble_nus.h>
#include <nrf_log.h>
#include <ble_conn_params.h>
#include <app_timer.h>
#include <fstorage.h>
#include "ble_support.h"
#include "ble_data_service.h"
#include <peer_manager.h>
#include <fds.h>
#include <app_scheduler.h>
#include "ecc.h"
#include "ble_adv_support.h"
#include "display_list.h"
#include "internal_command.h"
#include "marinedisplay.pb.h"
#include "display.h"


#define APP_FEATURE_NOT_SUPPORTED       (BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2)      /**< Reply when unsupported features are requested. */

#define DEVICE_NAME                     "MarineDisplay"                             /**< Name of device. Will be included in the advertising data. */

#define CENTRAL_LINK_COUNT              0
#define PERIPHERAL_LINK_COUNT           1
#define TX_POWER_LEVEL                  4                                           /**< Tx power in dBm */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)
#define SLAVE_LATENCY                   12
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(1000, 0)                    /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, 0)                    /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define DISCOVERABLE_TIMEOUT_SECONDS    60
#define DISCOVERABLE_TIMER_PERIOD       APP_TIMER_TICKS(1000, 0)


static uint16_t               m_conn_handle             = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
static ble_evt_handler_t      m_ble_evt_handler         = NULL;
static ble_support_callback_t m_disconnect_callback     = NULL;
static uint32_t               m_discoverable_start_time = 0;
APP_TIMER_DEF(m_discoverable_timer);

__ALIGN(4) static ble_gap_lesc_p256_sk_t m_lesc_sk;    /* LESC private key */
__ALIGN(4) static ble_gap_lesc_p256_pk_t m_lesc_pk;    /* LESC public key */
__ALIGN(4) static ble_gap_lesc_dhkey_t   m_lesc_dhkey; /* LESC DH Key */


static void ble_evt_dispatch(ble_evt_t * p_ble_evt);
static void on_ble_evt(ble_evt_t * p_ble_evt);


/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
 *          the device. It also sets the permissions and appearance.
 */
static void gap_params_init() {
  uint32_t                err_code;
  ble_gap_conn_params_t   gap_conn_params;
  ble_gap_conn_sec_mode_t sec_mode;

  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

  err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *) DEVICE_NAME, strlen(DEVICE_NAME));
  APP_ERROR_CHECK(err_code);

  memset(&gap_conn_params, 0, sizeof(gap_conn_params));

  gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
  gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
  gap_conn_params.slave_latency     = SLAVE_LATENCY;
  gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

  err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
  APP_ERROR_CHECK(err_code);
}


static void conn_params_error_handler(uint32_t nrf_error) {
  APP_ERROR_HANDLER(nrf_error);
}


static void conn_params_init(void) {
  uint32_t err_code;
  ble_conn_params_init_t cp_init;

  memset(&cp_init, 0, sizeof(cp_init));

  cp_init.p_conn_params = NULL;
  cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
  cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
  cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
  cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
  cp_init.disconnect_on_fail = true;
  cp_init.error_handler = conn_params_error_handler;

  err_code = ble_conn_params_init(&cp_init);
  APP_ERROR_CHECK(err_code);
}

static void sys_evt_dispatch(uint32_t sys_evt) {
  NRF_LOG_DEBUG("Got system event: %d\n", sys_evt)
  fs_sys_event_handler(sys_evt);
}


static void pm_evt_handler(pm_evt_t const * p_evt) {
  NRF_LOG_DEBUG("Peer Manager event: %d\n", p_evt->evt_id);
  switch(p_evt->evt_id) {
    case PM_EVT_STORAGE_FULL:
      NRF_LOG_INFO("Running GC for flash..\n");
      APP_ERROR_CHECK(fds_gc());
      break;
    case PM_EVT_CONN_SEC_CONFIG_REQ: {
      NRF_LOG_INFO("Allowing re-pairing\n");
      pm_conn_sec_config_t reply = { .allow_repairing = true };
      pm_conn_sec_config_reply(p_evt->conn_handle, &reply);
      break;
    }
    case PM_EVT_CONN_SEC_SUCCEEDED:
      display_list_clear();
      SCHED_INT_CMD(RENDER);
      break;
    default:
      break;
  }
}


static void render_discoverable_state(uint32_t secs_till_discoverable_timeout) {
  display_list_clear();

  DisplayCommand name_cmd = {
        .which_command = DisplayCommand_string_tag,
        .command.string = {0, 0, 25, 10}
    };
  strcpy(name_cmd.command.string.str, DEVICE_NAME);
  name_cmd.command.string.x = display_centered_x(name_cmd.command.string.str, name_cmd.command.string.font_size);
  display_list_add(0, &name_cmd);

  DisplayCommand cmd = {
        .which_command = DisplayCommand_string_tag,
        .command.string = {1, 0, 50, 8}
    };
  sprintf(cmd.command.string.str, "Discoverable %us", (unsigned int) secs_till_discoverable_timeout);
  cmd.command.string.x = display_centered_x(cmd.command.string.str, cmd.command.string.font_size);
  display_list_add(1, &cmd);
  SCHED_INT_CMD(RENDER);
}

static void render_pairing_passkey(char *passkey) {
  display_list_clear();
  DisplayCommand cmd = {
      .which_command = DisplayCommand_string_tag,
      .command.string = {0, 0, 15, 10}
  };
  strcpy(cmd.command.string.str, "Pairing PIN");
  cmd.command.string.x = display_centered_x(cmd.command.string.str, cmd.command.string.font_size);
  display_list_add(0, &cmd);

  DisplayCommand passkey_cmd = {
      .which_command = DisplayCommand_string_tag,
      .command.string = {1, 0, 50, 20}
  };
  strcpy(passkey_cmd.command.string.str, passkey);
  passkey_cmd.command.string.x = display_centered_x(passkey_cmd.command.string.str, passkey_cmd.command.string.font_size);
  display_list_add(1, &passkey_cmd);
  SCHED_INT_CMD(RENDER);
}

static void on_discoverable_stop() {
  app_timer_stop(m_discoverable_timer);
  display_list_clear();
  SCHED_INT_CMD(RENDER);
}

void on_discover_timer_tick(void *ctx) {
  uint32_t ticks_in_sec = APP_TIMER_TICKS(1000, 0);
  uint32_t duration     = 0;

  app_timer_cnt_diff_compute(app_timer_cnt_get(), m_discoverable_start_time, &duration);

  uint32_t ticks_till_discoverable_timeout = APP_TIMER_TICKS(1000 * DISCOVERABLE_TIMEOUT_SECONDS, 0) - duration;
  uint32_t secs_till_discoverable_timeout  = (ticks_till_discoverable_timeout + (ticks_in_sec / 2)) / ticks_in_sec;  // Round up

  if(secs_till_discoverable_timeout <= 0) {
    NRF_LOG_INFO("Discoverable timeout\n");
    ble_adv_start();
    on_discoverable_stop();
    SCHED_INT_CMD(DISPLAY_OFF);
  } else {
    render_discoverable_state(secs_till_discoverable_timeout);
  }
}


static void pairing_init() {
  ecc_init(true);

  APP_ERROR_CHECK(pm_init());
  APP_ERROR_CHECK(ecc_p256_keypair_gen(m_lesc_sk.sk, m_lesc_pk.pk));
  APP_ERROR_CHECK(pm_lesc_public_key_set(&m_lesc_pk));

  ble_gap_sec_params_t sec_param;
  memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

  // Security parameters to be used for all security procedures.
  sec_param.bond              = 1;
  sec_param.mitm              = 1;
  sec_param.lesc              = 1;
  sec_param.io_caps           = BLE_GAP_IO_CAPS_DISPLAY_ONLY;
  sec_param.min_key_size      = 7;
  sec_param.max_key_size      = 16;
  sec_param.kdist_own.enc     = 1;
  sec_param.kdist_own.id      = 1;
  sec_param.kdist_peer.enc    = 1;
  sec_param.kdist_peer.id     = 1;

  APP_ERROR_CHECK(pm_sec_params_set(&sec_param));
  APP_ERROR_CHECK(pm_register(pm_evt_handler));

  app_timer_create(&m_discoverable_timer, APP_TIMER_MODE_REPEATED, on_discover_timer_tick);
}


/**@brief Function for the SoftDevice initialization.
 *
 * @details This function initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init() {
  uint32_t err_code;
  nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

  // Initialize SoftDevice.
  SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

  ble_enable_params_t ble_enable_params;
  err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT, &ble_enable_params);
  ble_enable_params.gatts_enable_params.attr_tab_size = GATT_ATTR_TABLE_SIZE;
  APP_ERROR_CHECK(err_code);

  //Check the ram settings against the used number of links
  CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT,PERIPHERAL_LINK_COUNT);

  err_code = softdevice_enable(&ble_enable_params);
  APP_ERROR_CHECK(err_code);

  // Subscribe for BLE events.
  err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
  APP_ERROR_CHECK(err_code);

  // Subscribe for system events.
  err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
  APP_ERROR_CHECK(err_code);

  err_code = sd_ble_gap_tx_power_set(TX_POWER_LEVEL);
  APP_ERROR_CHECK(err_code);
}


static void ble_evt_dispatch(ble_evt_t * p_ble_evt) {
  on_ble_evt(p_ble_evt);
  ble_conn_params_on_ble_evt(p_ble_evt);
  if(m_ble_evt_handler != NULL) {
    m_ble_evt_handler(p_ble_evt);
  }
}


static void try_secure_connection() {
  pm_peer_id_t peer_id;
  if(pm_peer_id_get(m_conn_handle, &peer_id) == NRF_SUCCESS && peer_id != PM_PEER_ID_INVALID) {
    NRF_LOG_INFO("Known peer connected. Requesting connection security..\n");
    pm_conn_secure(m_conn_handle, false);
  }
}


static void on_ble_evt(ble_evt_t * p_ble_evt) {
  NRF_LOG_DEBUG("BLE event: %d\n", p_ble_evt->header.evt_id);

  uint32_t err_code;

  switch (p_ble_evt->header.evt_id) {
    case BLE_GAP_EVT_CONNECTED:
      m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
      NRF_LOG_INFO("Connected\n");
      try_secure_connection();
      on_discoverable_stop();
      ble_adv_stop();
      break; // BLE_GAP_EVT_CONNECTED

    case BLE_GAP_EVT_DISCONNECTED:
      m_conn_handle = BLE_CONN_HANDLE_INVALID;
      NRF_LOG_INFO("Disconnected\n");
      ble_adv_start();
      if(m_disconnect_callback != NULL) {
        m_disconnect_callback();
        m_disconnect_callback = NULL;
      }
      break; // BLE_GAP_EVT_DISCONNECTED

    case BLE_GATTS_EVT_SYS_ATTR_MISSING:
      // No system attributes have been stored.
      err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
      APP_ERROR_CHECK(err_code);
      break; // BLE_GATTS_EVT_SYS_ATTR_MISSING

    case BLE_GATTC_EVT_TIMEOUT:
      // Disconnect on GATT Client timeout event.
      err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
      APP_ERROR_CHECK(err_code);
      break; // BLE_GATTC_EVT_TIMEOUT

    case BLE_GATTS_EVT_TIMEOUT:
      // Disconnect on GATT Server timeout event.
      err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
      APP_ERROR_CHECK(err_code);
      break; // BLE_GATTS_EVT_TIMEOUT

    case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST: {
      ble_gatts_evt_rw_authorize_request_t  req;
      ble_gatts_rw_authorize_reply_params_t auth_reply;

      req = p_ble_evt->evt.gatts_evt.params.authorize_request;

      if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID) {
        if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
            (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
            (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL)) {
          if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE) {
            auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
          } else {
            auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
          }
          auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
          err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                                                     &auth_reply);
          APP_ERROR_CHECK(err_code);
        }
      }
    } break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

    case BLE_GAP_EVT_PASSKEY_DISPLAY:
      NRF_LOG_INFO("Passkey: %s\n", (uint32_t)p_ble_evt->evt.gap_evt.params.passkey_display.passkey);
      char buf[BLE_GAP_PASSKEY_LEN + 1];
      memcpy(buf, p_ble_evt->evt.gap_evt.params.passkey_display.passkey, BLE_GAP_PASSKEY_LEN);
      render_pairing_passkey(buf);
      break;

    case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
      APP_ERROR_CHECK(ecc_p256_shared_secret_compute(&m_lesc_sk.sk[0], &p_ble_evt->evt.gap_evt.params.lesc_dhkey_request.p_pk_peer->pk[0], &m_lesc_dhkey.key[0]));
      APP_ERROR_CHECK(sd_ble_gap_lesc_dhkey_reply(p_ble_evt->evt.gap_evt.conn_handle, &m_lesc_dhkey));
      break;

    case BLE_GAP_EVT_AUTH_STATUS:
      NRF_LOG_DEBUG("Auth status: %d %d\n", p_ble_evt->evt.gap_evt.params.auth_status.auth_status, p_ble_evt->evt.gap_evt.params.auth_status.error_src);
      break;

    default:
      break;
  }
}


void ble_support_init(ble_evt_handler_t ble_evt_handler) {
  m_ble_evt_handler = ble_evt_handler;
  ble_stack_init();
  gap_params_init();
  conn_params_init();
}


void ble_support_advertising_init() {
  pairing_init();
  ble_adv_init();
}


void ble_support_advertising_start() {
  ble_adv_start();
}


void ble_support_disconnect(ble_support_callback_t callback) {
  if(m_conn_handle != BLE_CONN_HANDLE_INVALID) {
    m_disconnect_callback = callback;
    sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
  } else if(callback != NULL) {
    callback();
  }
}


void ble_support_start_discoverable() {
  ble_adv_discoverable_start();
  app_timer_start(m_discoverable_timer, DISCOVERABLE_TIMER_PERIOD, NULL);
  m_discoverable_start_time = app_timer_cnt_get();
  NRF_LOG_INFO("Discoverable\n")
  render_discoverable_state(DISCOVERABLE_TIMEOUT_SECONDS);
}


void ble_support_factory_reset() {
  APP_ERROR_CHECK(pm_peers_delete());
  ble_support_disconnect(NULL);
  ble_adv_start();
}
