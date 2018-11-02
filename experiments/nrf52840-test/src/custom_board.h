#pragma once

#define LEDS_NUMBER    5
#define LED_1          18
#define LED_2          19
#define LED_3          20
#define LED_4          21
#define LED_5          22
#define LEDS_LIST { LED_1, LED_2, LED_3, LED_4, LED_5 }
#define LEDS_ACTIVE_STATE 1

#define BSP_LED_0      LED_1
#define BSP_LED_1      LED_2
#define BSP_LED_2      LED_3
#define BSP_LED_3      LED_4
#define BSP_LED_4      LED_5

#define BUTTONS_NUMBER 2
#define BUTTON_1       16
#define BUTTON_2       17
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_1, BUTTON_2 }

#define BSP_BUTTON_0   BUTTON_1
#define BSP_BUTTON_1   BUTTON_2
