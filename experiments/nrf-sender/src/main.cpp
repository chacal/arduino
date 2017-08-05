#include "RF24.h"
#include "main.h"


#define NRF_CE A0                     // Chip Enbale pin for NRF radio
#define NRF_CSN 3                     // SPI Chip Select for NFR radio


RF24 radio(NRF_CE, NRF_CSN);
uint8_t address[6] = "gwnod";

void setup() {
  Serial.begin(57600);
  initializeRadio();
}

void loop() {
  Serial.println("Sending..");
  char data[5];
  data[0] = 'g';
  data[1] = 1;
  data[2] = 'a';
  data[3] = 'b';
  data[4] = 'c';
  radio.powerUp();
  radio.write(&data, sizeof(data));
  radio.powerDown();

  delay(300);
}


void initializeRadio() {
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setPayloadSize(32);
  radio.enableDynamicPayloads();
  radio.openWritingPipe(address);
  radio.setAutoAck(true);
  radio.setRetries(1, 10);  // Retry every 250us for 10 times + send times (~1ms)
}
