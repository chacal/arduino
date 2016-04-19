#include <STM32Sleep.h>
#include <RTClock.h>
#include <SPI.h>
#include "RF24_STM32.h"

/*
 * Measurements:
 *  - Sleep current 4µA
 *  - Run current 30mA
 *  - Loop run period 1,8 ms
 */

RTClock rt(RTCSEL_LSE);
// Set up nRF24L01 radio on SPI-1 bus (MOSI-PA7, MISO-PA6, SCLK-PA5)
RF24 radio(PB0,PB1);
uint8_t address[6] = "1Node";
long int alarmDelay = 3;

void setup() {
  adc_disable_all();
  disableAllPeripheralClocks();
  rcc_clk_enable(RCC_SPI1);
  rcc_clk_enable(RCC_TIMER1);
  rcc_clk_enable(RCC_PWR);
  rcc_clk_enable(RCC_GPIOA);
  rcc_clk_enable(RCC_GPIOB);
  rcc_clk_enable(RCC_GPIOC);

  initializeRadio();
}

void loop() {
  sendData();
  sleepAndWakeUp(STANDBY, &rt, alarmDelay);
}

void initializeRadio() {
  SPI.begin();

  // Setup and configure rf radio
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.enableDynamicPayloads();
  radio.setAutoAck(false);  // Don't wait for ACKs to save power
  radio.setPALevel(RF24_PA_MAX);
  radio.setCRCLength(RF24_CRC_16);
  radio.openWritingPipe(address);
}

void sendData() {
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
}

