#include <stdbool.h>
#include <stdint.h>
#include <ble/common/ble_advdata.h>
#include <nrf_drv_adc.h>
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "sdk_config_nrf51/sdk_config.h"

#define CENTRAL_LINK_COUNT              0                                 /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           0                                 /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/

#define APP_CFG_NON_CONN_ADV_TIMEOUT    0                                 /**< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables timeout. */
#define NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(3000, UNIT_0_625_MS) /**< The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100ms to 10.24s). */

#define DEVICE_NAME                     "S035"

#define APP_TIMER_PRESCALER             0                                 /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                 /**< Size of timer operation queues. */

#define TX_POWER_LEVEL                  4                                 /** Max power, +4dBm */

static ble_gap_adv_params_t  m_adv_params;                                 /**< Parameters to be passed to the stack when starting advertising. */
static nrf_drv_adc_channel_t m_adc_channel_config;
static uint16_t              m_vcc;

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
  sensor_data.vcc         = 4015;
  manuf_data.data.p_data  = (uint8_t *) &sensor_data;
  manuf_data.data.size    = sizeof(sensor_data);

  memset(&advdata, 0, sizeof(advdata));

  advdata.name_type             = BLE_ADVDATA_FULL_NAME;
  advdata.flags                 = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
  advdata.p_manuf_specific_data = &manuf_data;

  err_code = ble_advdata_set(&advdata, NULL);
  APP_ERROR_CHECK(err_code);

  // Initialize advertising parameters (used when starting advertising).
  memset(&m_adv_params, 0, sizeof(m_adv_params));

  m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
  m_adv_params.p_peer_addr = NULL;                             // Undirected advertisement.
  m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
  m_adv_params.interval    = NON_CONNECTABLE_ADV_INTERVAL;
  m_adv_params.timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT;
}


static void advertising_start(void) {
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

static void adc_sample(void *ctx) {
  nrf_adc_value_t value;
  nrf_drv_adc_sample_convert(&m_adc_channel_config, &value);
  m_vcc = value / (float) 1023 * 3 * 1.2 * 1000;  // ADC value * prescale (1/3) * 1.2V reference * mV
  NRF_LOG_INFO("VCC: %d\n", m_vcc);
}

static void adc_init(void) {
  nrf_drv_adc_config_t config = NRF_DRV_ADC_DEFAULT_CONFIG;
  APP_ERROR_CHECK(nrf_drv_adc_init(&config, NULL));

  m_adc_channel_config.config.config.input              = NRF_ADC_CONFIG_SCALING_SUPPLY_ONE_THIRD;
  m_adc_channel_config.config.config.external_reference = NRF_ADC_CONFIG_REF_VBG;
  m_adc_channel_config.config.config.resolution         = NRF_ADC_CONFIG_RES_10BIT;

  nrf_drv_adc_channel_enable(&m_adc_channel_config);

  APP_TIMER_DEF(adc_timer);
  app_timer_create(&adc_timer, APP_TIMER_MODE_REPEATED, adc_sample);
  app_timer_start(adc_timer, APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER), NULL);
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
  adc_init();

  NRF_LOG_INFO("BLE Beacon started\n");
  advertising_start();

  for(;;) {
    power_manage();
  }
}
