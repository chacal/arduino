#ifndef AUTOPILOTREMOTERFM69_MAIN_H
#define AUTOPILOTREMOTERFM69_MAIN_H

#include "Arduino.h"
#include <power.h>

#define MIN_AWAKE_TIME_MS    15  // This needs to be more than Bounce2 library's default debounce time (10ms)
#define DEBUG                0
#define RFM_RECEIVER_ID      1  // Gateway is ID 1

// Measured resistances for the voltage divider resistors
#define R1                   2180000
#define R2                   1000000
#define AREF                 3.3  // Regulated 3V used as board's VCC

#define RFM69_NSS_PIN        8
#define NETWORKID            50
#define INSTANCE             10
#define CONFIG_EEPROM_ADDR   0
#define FORCE_CONFIG_SAVE    0

#define FREQUENCY            RF69_433MHZ
#define SERIAL_BAUD          57600

struct Config {
  long r1;
  long r2;
  uint8_t instance;
};


void initializeConfig();
void initializeRadio();
bool hasMinAwakeTimeElapsed();
void goToSleep();

#endif //AUTOPILOTREMOTERFM69_MAIN_H
