#include <STM32Sleep.h>
#include <RTClock.h>

RTClock rt(RTCSEL_LSE);
long int alarmDelay = 5;

void setup() {
  adc_disable_all();  // Disable ADC to save power
  setGPIOModeToAllPins(GPIO_INPUT_ANALOG);  // Set all GPIO pins to Analog input to save power (this disables pretty much all I/O incl. Serial)
  sleepAndWakeUp(STOP, &rt, alarmDelay);
}

void loop() {
  // We have woken up from sleep! System clock is set to 8MHz HSI.
  
  // Optionally disable peripheral clocks to save power while in Run mode
  disableAllPeripheralClocks();

  delay(100);  // As the main clock is only 8MHz instead of normal 72MHz this actually takes 900ms

  sleepAndWakeUp(STOP, &rt, alarmDelay);
}

