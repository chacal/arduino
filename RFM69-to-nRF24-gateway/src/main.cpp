#include <Arduino.h>
#include <RFM69.h>
#include <SPI.h>
#include <RFM69registers.h>
#include "RF24.h"
#include "main.hpp"
#include <EEPROM.h>

#define RFM_NETWORK_ID        50
#define RFM_FREQUENCY         RF69_433MHZ
#define RFM_NETWORK_INSTANCE  1    // Use 1 as gateway address
#define NRF_CE               A0    // Chip Enable pin for NRF radio
#define NRF_CSN               3    // SPI Chip Select for NFR radio
#define RFM69_NSS            10    // SPI Chip Select / NSS for RFM69
#define RFM69_IRQ_PIN         2    // IRQ pin for RFM69
#define RFM69_IRQ_NUM         0    // Pin 2 is EXT_INT0
uint8_t address[6] =         "1Node";

#define SERIAL_BAUD           57600
#define FORCE_CONFIG_SAVE     0
#define CONFIG_EEPROM_ADDR    0


RFM69 rfm69(RFM69_NSS, RFM69_IRQ_PIN, true, RFM69_IRQ_NUM);
RF24 nrf(NRF_CE, NRF_CSN);


struct Config {
  uint8_t instance;
} config;


void setup() {
  Serial.begin(SERIAL_BAUD);
  initializeConfig();
  initializeRfmRadio();
  initializeNrfRadio();
}

void loop() {
  if (rfm69.receiveDone())
  {
    unsigned long start = micros();

    bool ret = nrf.write((void*)rfm69.DATA, rfm69.DATALEN);
    if (rfm69.ACKRequested() && ret) {
      rfm69.sendACK();
    }
    unsigned long duration = micros() - start;

    Serial.print(ret);
    Serial.print(" ");
    Serial.print(rfm69.RSSI);
    Serial.print(" ");
    Serial.println(duration);
  }
}



void initializeNrfRadio() {
  nrf.begin();
  nrf.setPALevel(RF24_PA_MAX);
  nrf.setDataRate(RF24_250KBPS);
  nrf.setPayloadSize(32);
  nrf.enableDynamicPayloads();
  nrf.openWritingPipe(address);
  // Use auto ACKs to avoid sleeping between radio transmissions
  nrf.setAutoAck(true);
  nrf.setRetries(1, 15);  // Retry every 1ms for maximum of 3ms + send times (~1ms)
}

void initializeRfmRadio() {
  rfm69.initialize(RFM_FREQUENCY, RFM_NETWORK_INSTANCE, RFM_NETWORK_ID);
  rfm69.setHighPower();
  rfm69.setPowerLevel(0);
}

void initializeConfig() {
  EEPROM.get(CONFIG_EEPROM_ADDR, config);
  if(FORCE_CONFIG_SAVE || (config.instance == -1 || config.instance == 255)) {
    config.instance = RFM_NETWORK_INSTANCE;
    Serial.println("Saving defaults to EEPROM...");
    EEPROM.put(CONFIG_EEPROM_ADDR, config);
  }
  Serial.println("Used configuration:");
  Serial.print("Instance = ");
  Serial.println(config.instance);
  Serial.flush();
}
