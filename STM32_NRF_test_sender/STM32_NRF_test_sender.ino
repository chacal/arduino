#include <SPI.h>
#include "RF24_STM32.h"

// Set up nRF24L01 radio on SPI-1 bus (MOSI-PA7, MISO-PA6, SCLK-PA5)
RF24 radio(PB0,PB1);
uint8_t address[6] = "1Node";

void setup(){
//  Serial.begin(115200);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);

  // Setup and configure rf radio
  radio.begin();
  radio.enableDynamicPayloads();
  radio.setAutoAck(false);  // Don't wait for ACKs to save power
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setCRCLength(RF24_CRC_16);
  radio.openWritingPipe(address);

//  radio.printDetails();
}

void loop(){
  uint8_t data[8];
  memset(data, 0, 8);
  data[0] = 't';
  data[1] = 4;

  float temp = 12.3456;
  uint16_t vcc = 4000;
  memcpy(&data[2], &temp, sizeof(temp));
  memcpy(&data[6], &vcc, sizeof(vcc));
    
  radio.powerUp();
  bool success = radio.write(data, sizeof(data));
  radio.powerDown();

  delay(5000);
}



