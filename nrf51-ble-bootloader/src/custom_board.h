#ifndef NRF51_BLE_BOOTLOADER_CUSTOM_BOARD_H
#define NRF51_BLE_BOOTLOADER_CUSTOM_BOARD_H

#define LEDS_NUMBER    2
#define LED_1          3
#define LED_2          4
#define LEDS_LIST { LED_1, LED_2 }
#define LEDS_ACTIVE_STATE 1
#define BSP_LED_0      LED_1
#define BSP_LED_1      LED_2


#define BUTTONS_NUMBER 1
#define SW_1           0
#define BUTTONS_LIST { SW_1 }
#define BUTTONS_ACTIVE_STATE 0
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP
#define BSP_BUTTON_3   SW_1


// Low frequency clock source to be used by the SoftDevice
#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}


#endif //NRF51_BLE_BOOTLOADER_CUSTOM_BOARD_H
