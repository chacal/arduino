#pragma once

#define DCDC_S201    DCDC_MODE_DISABLED
#define DCDC_S202    DCDC_MODE_DISABLED
#define DCDC_S203    DCDC_MODE_DISABLED
#define DCDC_S204    DCDC_MODE_DISABLED
#define DCDC_S205    DCDC_MODE_DISABLED
#define DCDC_S206    DCDC_MODE_DISABLED
#define DCDC_S207    DCDC_MODE_DISABLED
#define DCDC_S208    DCDC_MODE_DISABLED
#define DCDC_S209    DCDC_MODE_DISABLED
#define DCDC_S210    DCDC_MODE_DISABLED
#define DCDC_S211    DCDC_MODE_DISABLED
#define DCDC_S212    DCDC_MODE_DISABLED
#define DCDC_S213    DCDC_MODE_DISABLED
#define DCDC_S214    DCDC_MODE_DISABLED
#define DCDC_S215    DCDC_MODE_DISABLED
#define DCDC_S216    DCDC_MODE_ENABLED
#define DCDC_S217    DCDC_MODE_ENABLED
#define DCDC_S218    DCDC_MODE_ENABLED

#define DCDC_P300    DCDC_MODE_DISABLED
#define DCDC_P301    DCDC_MODE_DISABLED
#define DCDC_P302    DCDC_MODE_ENABLED
#define DCDC_P303    DCDC_MODE_ENABLED
#define DCDC_P304    DCDC_MODE_ENABLED
#define DCDC_P305    DCDC_MODE_ENABLED
#define DCDC_P306    DCDC_MODE_ENABLED
#define DCDC_P307    DCDC_MODE_ENABLED
#define DCDC_P308    DCDC_MODE_ENABLED

#define DCDC_C400    DCDC_MODE_DISABLED
#define DCDC_T100    DCDC_MODE_ENABLED
#define DCDC_W100    DCDC_MODE_ENABLED


#define DCDC_MODE_DISABLED         NRF_POWER_DCDC_DISABLE
#define DCDC_MODE_ENABLED          NRF_POWER_DCDC_ENABLE
#define DCDC_MODE_BY_DEVICE_NAME   2

#define _STR(x) #x
#define STR(x)  _STR(x)

#define _DCDC_MODE_FOR_NAME(n) DCDC_##n
#define DCDC_MODE_FOR_NAME(n) _DCDC_MODE_FOR_NAME(n)


void dcdc_init(uint8_t mode) {
  NRF_LOG_INFO("Setting DCDC mode to %d", mode)
  APP_ERROR_CHECK(sd_power_dcdc_mode_set(mode));
}