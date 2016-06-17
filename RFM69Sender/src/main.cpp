#include <Arduino.h>
#include <RFM69.h>    //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>
#include <power.h>
#include <RFM69registers.h>

#define NETWORKID     100  //the same on all nodes that talk to each other
#define NODEID        1

#define FREQUENCY     RF69_433MHZ
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!

#define SERIAL_BAUD   57600

RFM69 radio(RF69_SPI_CS, RF69_IRQ_PIN, true);

void setup() {
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower();
  radio.encrypt(ENCRYPTKEY);
  // radio.setPowerLevel(0);
  // radio.writeReg(REG_BITRATEMSB, RF_BITRATEMSB_300000);
  // radio.writeReg(REG_BITRATELSB, RF_BITRATELSB_300000);

  radio.sleep();

  char buff[50];
  sprintf(buff, "\nSending at %d Mhz...", 433);
  Serial.println(buff);
  Serial.flush();
}

void loop() {
  long start = micros();

  // radio.send(2, "Here is some longer test data", 29, false);
  if(radio.sendWithRetry(2, "Here is some longer test data", 29, 2)) {
    long duration = micros() - start;
    Serial.println(duration);
  } else {
    Serial.println("failed");
  }

  radio.sleep();

  Serial.flush();

  powerDown(WAKEUP_DELAY_2_S);
}
