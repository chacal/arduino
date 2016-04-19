#include <SPI.h>
#include "RF24.h"
#include <printf.h>

// These PINs can be used with standard Pro Mini (custom board uses different pins)
#define NRF_CE 9                      // Chip Enbale pin for NRF radio
#define NRF_CSN 10                    // SPI Chip Select for NFR radio
#define INSTANCE 5

RF24 radio(NRF_CE, NRF_CSN);
uint8_t address[6] = "1Node";

struct {
  char tag;
  uint8_t instance;
  float temp;
  int vcc;
} measurements;


void setup() {
  Serial.begin(115200);
  printf_begin();
  measurements.tag = 't';
  initializeRadio();
  radio.printDetails();
}

void loop() {
  // put your main code here, to run repeatedly:
  measurements.temp = 25.4;
  measurements.instance = INSTANCE;
  measurements.vcc = 3500;

  radio.powerUp();
  radio.write(&measurements, sizeof(measurements));
  radio.powerDown();

  delay(1000);
}


void initializeRadio() {
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setPayloadSize(32);
  radio.enableDynamicPayloads();
  radio.openWritingPipe(address);
  radio.setRetries(15, 15);
  radio.setAutoAck(true);
}

