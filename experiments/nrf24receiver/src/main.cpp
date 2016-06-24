#include <Arduino.h>
#include <SPI.h>
#include <power.h>
#include "RF24.h"
#include <printf.h>

#define NRF_CE        9    // Chip Enbale pin for NRF radio
#define NRF_CSN       10    // SPI Chip Select for NFR radio

void initializeNrfRadio();

RF24 nrf(NRF_CE, NRF_CSN);
uint8_t address[6] = "1Node";

uint8_t rcvbuf[40];
unsigned long prev;

void setup() {
  Serial.begin(115200);
  printf_begin();
  initializeNrfRadio();
  nrf.startListening();
}


void loop() {
  if (nrf.available())  {
    uint8_t len;

    while (nrf.available())
    {
      // Fetch the payload, and see if this was the last one.
      len = nrf.getDynamicPayloadSize();
      nrf.read(rcvbuf, len);

      if(len > 4) {
        break;
      }

      unsigned long cur = *(unsigned long*)rcvbuf;

      // Spew it
      printf("Got payload size=%i value=%ld\n", len, cur);
      if(cur - prev > 1) {
        printf("Missed packet! %ld\n", cur);
      }
      prev = cur;

      nrf.writeAckPayload(1, &prev, sizeof(prev));
    }
  }
}


void initializeNrfRadio() {
  nrf.begin();
  nrf.setPALevel(RF24_PA_MAX);
  nrf.setDataRate(RF24_2MBPS);
  nrf.setPayloadSize(32);
  nrf.enableDynamicPayloads();
  nrf.enableAckPayload();
  nrf.openReadingPipe(1, address);
  // Use auto ACKs to avoid sleeping between radio transmissions
  nrf.setAutoAck(true);
  nrf.setRetries(5, 15);  // Retry every 1ms for maximum of 3ms + send times (~1ms)
}
