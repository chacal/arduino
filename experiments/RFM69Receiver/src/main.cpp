#include <Arduino.h>
#include <RFM69.h>    //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>
#include <power.h>
#include <RFM69registers.h>

#define NETWORKID            50
#define NODEID               100
#define RFM69_NSS            10    // SPI Chip Select / NSS for RFM69
#define RFM69_IRQ_PIN         2    // IRQ pin for RFM69
#define RFM69_IRQ_NUM         0    // Pin 2 is EXT_INT0
#define FREQUENCY            RF69_433MHZ
#define SERIAL_BAUD          57600

RFM69 radio(RFM69_NSS, RFM69_IRQ_PIN, true);

void setup() {
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower();
  radio.setPowerLevel(0);

  Serial.println("Listening at 433 Mhz...");
}

void loop() {
  if (radio.receiveDone())
  {
    uint8_t size = radio.DATALEN;
    uint8_t buf[size];
    memcpy(buf, (const void*)radio.DATA, size);

    if (radio.ACKRequested())
    {
      unsigned long start = micros();
      radio.sendACK();
      unsigned long duration = micros() - start;
      Serial.print("ACK sent in ");
      Serial.print(duration);
      Serial.print(" us. ");
    }

    Serial.print("Sender node: ");Serial.print(radio.SENDERID);Serial.print(" ");
    Serial.print(" Data: [");
    for(uint8_t i = 0; i < size; i++) {
      Serial.print((char)buf[i]);
    }
    Serial.print("]  RX_RSSI:");Serial.print(radio.RSSI);Serial.println("");
  }
}
