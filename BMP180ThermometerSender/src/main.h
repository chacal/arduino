#include <power.h>

void initializeConfig();
void initializeRadio();
void readTempAndPressure(double &temp, double &pressure, float tempCalibration, float pressureCalibration);
int readRawVcc();
void measureTimeAndSleep(WatchdogTimerPrescaler delay1, WatchdogTimerPrescaler delay2 = WAKEUP_DELAY_NO_SLEEP);
