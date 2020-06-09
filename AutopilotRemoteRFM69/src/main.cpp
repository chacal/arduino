#include <SPI.h>
#include <RFM69.h>
#include "main.h"
#include "buttons.h"
#include <EEPROM.h>
#include <LowPower.h>

static RFM69 radio(RFM69_NSS_PIN, RF69_IRQ_PIN, true);
static Config config;
unsigned long wakeUpTime = 0;


void setup() {
  Serial.begin(SERIAL_BAUD);
  initializeConfig();
  initializeRadio();
  setupPinChangeInterrupts();
  setupButtonCallbacks(&radio, &config);
  goToSleep();
}


void loop() {
  updateButtonStates();

  if(hasMinAwakeTimeElapsed() && noButtonsPressed()) {
    goToSleep();
  }
}


bool hasMinAwakeTimeElapsed() { return millis() - wakeUpTime > MIN_AWAKE_TIME_MS; }

void goToSleep() {
  if(DEBUG) {
    Serial.println("Sleeping");
    Serial.flush();
  }
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  wakeUpTime = millis();
}


void initializeRadio() {
  radio.initialize(FREQUENCY, INSTANCE, NETWORKID);
  radio.setHighPower();
  radio.setPowerLevel(20);
  radio.sleep();
}

void initializeConfig() {
  EEPROM.get(CONFIG_EEPROM_ADDR, config);
  if(FORCE_CONFIG_SAVE || (config.r1 == -1 && config.r2 == -1)) {
    config.r1 = R1;
    config.r2 = R2;
    config.instance = INSTANCE;
    Serial.println("Saving defaults to EEPROM...");
    EEPROM.put(CONFIG_EEPROM_ADDR, config);
  }
  Serial.println("Used configuration:");
  Serial.print("R1 = ");
  Serial.println(config.r1);
  Serial.print("R2 = ");
  Serial.println(config.r2);
  Serial.print("Instance = ");
  Serial.println(config.instance);
  Serial.flush();
}
