#include <Arduino.h>
#include <RFM69.h>
#include <SPI.h>
#include <RFM69registers.h>
#include "RF24.h"
#include "main.hpp"
#include <EEPROM.h>

#define DEBUG                 0

#define RFM_NETWORK_ID        50
#define RFM_FREQUENCY         RF69_433MHZ
#define RFM_NETWORK_INSTANCE  1    // Use 1 as gateway address
#define NRF_CE               A0    // Chip Enable pin for NRF radio
#define NRF_CSN               3    // SPI Chip Select for NFR radio
#define RFM69_NSS            10    // SPI Chip Select / NSS for RFM69
#define RFM69_IRQ_PIN         2    // IRQ pin for RFM69
#define RFM69_IRQ_NUM         0    // Pin 2 is EXT_INT0
uint8_t nrfServerAddress[6] = "1Node";    // Packets from RFM69 are forwarded to this address
uint8_t listenAddress[6]    = "gwnod";    // This address is listened and incoming packets are forwarded to RFM69

#define SERIAL_BAUD           57600
#define FORCE_CONFIG_SAVE     0
#define CONFIG_EEPROM_ADDR    0


RFM69 rfm69(RFM69_NSS, RFM69_IRQ_PIN, true, RFM69_IRQ_NUM);
RF24 nrf(NRF_CE, NRF_CSN);


struct Config {
  uint8_t instance;
} config;

struct RFMGatewayData {
  char tag;
  uint8_t instance;
  int rssi;
  bool ackSent;
  unsigned long previousSampleTimeMicros;
} gwData;

void setup() {
  Serial.begin(SERIAL_BAUD);
  initializeConfig();
  initializeRfmRadio();
  initializeNrfRadio();
  nrf.startListening();
  gwData.tag = 's';
  gwData.instance = RFM_NETWORK_INSTANCE;
}

void loop() {
  if (rfm69.receiveDone() && rfm69.DATALEN > 0)
  {
    unsigned long start = micros();

    noInterrupts();
    nrf.stopListening();
    bool ret = nrf.write((void*)rfm69.DATA, rfm69.DATALEN);
    interrupts();

    if (rfm69.ACKRequested() && ret) {
      gwData.ackSent = true;
      rfm69.sendACK();
    } else {
      gwData.ackSent = false;
    }
    unsigned long duration = micros() - start;

    gwData.rssi = rfm69.RSSI;
    gwData.previousSampleTimeMicros = duration;
    delay(20);  // Give nRF gateway some time to process the previous packet

    noInterrupts();
    nrf.write(&gwData, sizeof(gwData));
    nrf.startListening();
    interrupts();

#if DEBUG
    Serial.print(ret);
    Serial.print(" ");
    Serial.print(rfm69.RSSI);
    Serial.print(" ");
    Serial.println(duration);
#endif
  }

  noInterrupts();
  if(nrf.available()) {
    uint8_t size = nrf.getDynamicPayloadSize();
    uint8_t buf[size];
    nrf.read(&buf, size);
    interrupts();

    #if DEBUG
      Serial.print("Got data from nRF: [");
      for(int i = 0; i < size; ++i) {
        Serial.print(buf[i]);
      }
      Serial.println(']');
    #endif

    if(size > 2 && buf[0] == 'g') {
      uint8_t rfmToAddress = buf[1];
      bool acked = rfm69.sendWithRetry(rfmToAddress, buf+2, size-2, 3, 5);

      #if DEBUG
        Serial.print("Sent data to RFM node: "); Serial.print(rfmToAddress);
        Serial.print(" Got ACK: "); Serial.print(acked);
        Serial.print(" RSSI: "); Serial.println(rfm69.RSSI);
      #endif
    }
  } else {
    interrupts();
  }
}



void initializeNrfRadio() {
  nrf.begin();
  nrf.setPALevel(RF24_PA_MAX);
  nrf.setDataRate(RF24_250KBPS);
  nrf.setPayloadSize(32);
  nrf.enableDynamicPayloads();
  nrf.openWritingPipe(nrfServerAddress);
  nrf.openReadingPipe(1, listenAddress);
  // Use auto ACKs to avoid sleeping between radio transmissions
  nrf.setAutoAck(true);
  nrf.setRetries(1, 15);  // Retry every 1ms for maximum of 3ms + send times (~1ms)
}

void initializeRfmRadio() {
  rfm69.initialize(RFM_FREQUENCY, RFM_NETWORK_INSTANCE, RFM_NETWORK_ID);
  rfm69.setHighPower();
  rfm69.setPowerLevel(20);
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
