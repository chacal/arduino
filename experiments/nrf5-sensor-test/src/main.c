#include <stdbool.h>
#include <stdint.h>
#include <ble/common/ble_advdata.h>
#include <nrf_drv_twi.h>
#include <app_util_platform.h>
#include "softdevice_handler.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "bmp180.h"
#include "vcc_measurement.h"

#ifdef NRF51
#include "sdk_config_nrf51/sdk_config.h"
#define PIN_SCA 0
#define PIN_SCL 1
#else
#include "sdk_config_nrf52/sdk_config.h"
#define PIN_SCA 11
#define PIN_SCL 12
#endif

#define CENTRAL_LINK_COUNT              0                                 /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           0                                 /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define APP_CFG_NON_CONN_ADV_TIMEOUT    0                                 /**< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables timeout. */
#define NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(3000, UNIT_0_625_MS) /**< The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100ms to 10.24s). */

#define DEVICE_NAME                     "S035"

#define APP_TIMER_PRESCALER             0                                 /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                 /**< Size of timer operation queues. */

#define TX_POWER_LEVEL                  4                                 /** Max power, +4dBm */

static uint16_t              m_vcc;
static nrf_drv_twi_t         m_twi = NRF_DRV_TWI_INSTANCE(0);

#pragma pack(1)

typedef struct {
  uint8_t  ttl;
  uint16_t tag;
  int16_t  temperature;
  uint16_t humidity;
  uint16_t pressure;
  uint16_t vcc;
}                            sensor_data_t;

static void advertising_init(void) {
  uint32_t err_code;

  ble_gap_conn_sec_mode_t sec_mode;
  BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&sec_mode);

  err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *) DEVICE_NAME, strlen(DEVICE_NAME));
  APP_ERROR_CHECK(err_code);


  ble_advdata_t advdata;

  ble_advdata_manuf_data_t manuf_data;
  manuf_data.company_identifier = 0xDADA;

  sensor_data_t sensor_data;
  sensor_data.ttl         = 2;
  sensor_data.tag         = 'm';
  sensor_data.temperature = 2150;
  sensor_data.humidity    = 5505;
  sensor_data.pressure    = 10153;
  sensor_data.vcc         = m_vcc;
  manuf_data.data.p_data  = (uint8_t *) &sensor_data;
  manuf_data.data.size    = sizeof(sensor_data);

  memset(&advdata, 0, sizeof(advdata));

  advdata.name_type             = BLE_ADVDATA_FULL_NAME;
  advdata.flags                 = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
  advdata.p_manuf_specific_data = &manuf_data;

  err_code = ble_advdata_set(&advdata, NULL);
  APP_ERROR_CHECK(err_code);
}


static void advertising_start(void) {
  ble_gap_adv_params_t  m_adv_params;
  memset(&m_adv_params, 0, sizeof(m_adv_params));

  m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
  m_adv_params.p_peer_addr = NULL;                             // Undirected advertisement.
  m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
  m_adv_params.interval    = NON_CONNECTABLE_ADV_INTERVAL;
  m_adv_params.timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT;
  APP_ERROR_CHECK(sd_ble_gap_adv_start(&m_adv_params));
}

static void ble_stack_init(void) {
  nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;
  SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

  ble_enable_params_t ble_enable_params;
  APP_ERROR_CHECK(softdevice_enable_get_default_config(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT, &ble_enable_params));

  CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT);

  APP_ERROR_CHECK(softdevice_enable(&ble_enable_params));
  APP_ERROR_CHECK(sd_ble_gap_tx_power_set(TX_POWER_LEVEL));
}

static void on_vcc_measurement(uint16_t vcc) {
  m_vcc = vcc;
  advertising_init();   // Update advertising data
}

static void twi_init(void) {
  const nrf_drv_twi_config_t twi_config = {
      .scl                = PIN_SCL,
      .sda                = PIN_SCA,
      .frequency          = NRF_TWI_FREQ_400K,
      .interrupt_priority = APP_IRQ_PRIORITY_LOW,
      .clear_bus_init     = false
  };

  APP_ERROR_CHECK(nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL));
  nrf_drv_twi_enable(&m_twi);
}


static void power_manage(void) {
  uint32_t err_code = sd_app_evt_wait();
  APP_ERROR_CHECK(err_code);
}


int main(void) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));

  APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
  ble_stack_init();
  advertising_init();
  vcc_measurement_init(30000, on_vcc_measurement);
  twi_init();
  bmp180_init(&m_twi);

  NRF_LOG_INFO("BLE Beacon started\n");
  advertising_start();

  bmp180_get_temp_and_pressure();

  for(;;) {
    power_manage();
  }
}
