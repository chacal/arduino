#include <power.h>

void initializeConfig();
void initializeRadio();
void measureAndRead(float &temp, float &pressure, float &humidity, float tempCalibration);
int readRawVcc();
void measureTimeAndSleep(WatchdogTimerPrescaler delay1, WatchdogTimerPrescaler delay2 = WAKEUP_DELAY_NO_SLEEP);
