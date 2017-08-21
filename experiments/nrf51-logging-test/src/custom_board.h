#pragma once

#define LEDS_NUMBER    1
#define LED_1          4
#define LEDS_LIST { LED_1 }
#define LEDS_ACTIVE_STATE 1
#define BSP_LED_0      LED_1

#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}
