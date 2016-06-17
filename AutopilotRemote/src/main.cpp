#include "Arduino.h"
#include "RF24.h"
#include "main.h"
#include "buttons.h"

/* Pins */
#define NRF_CE 9                      // Chip Enbale pin for NRF radio
#define NRF_CSN 10                     // SPI Chip Select for NFR radio

static RF24 radio(NRF_CE, NRF_CSN);
uint8_t address[6] = "1Node";
unsigned long wakeUpTime = 0;


void setup() {
  Serial.begin(57600);
  initializeRadio();
  setupPinChangeInterrupts();
  setupButtonCallbacks(&radio);
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
  powerDown();
  wakeUpTime = millis();
}

void initializeRadio() {
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setPayloadSize(32);
  radio.enableDynamicPayloads();
  radio.openWritingPipe(address);
  // Use auto ACKs to avoid sleeping between radio transmissions
  radio.setAutoAck(true);
  radio.setRetries(3, 10);  // Retry every 1ms for maximum of 10 times
}
