#include <power.h>

void initializeConfig();
void initializeRadio();
void measureAndRead(float &temp, float &pressure, float &humidity, float tempCalibration, float pressureCalibration);
void measureTimeAndSleep(WatchdogTimerPrescaler delay1, WatchdogTimerPrescaler delay2 = WAKEUP_DELAY_NO_SLEEP);
