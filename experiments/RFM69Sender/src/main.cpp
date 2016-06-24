#include <Arduino.h>
#include <RFM69.h>    //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>
#include <power.h>
#include <RFM69registers.h>

#define NETWORKID     50  //the same on all nodes that talk to each other
#define INSTANCE      9
#define RECEIVER_ID   1  // Gateway is ID 1

#define FREQUENCY     RF69_433MHZ

#define SERIAL_BAUD   57600

RFM69 radio(RF69_SPI_CS, RF69_IRQ_PIN, true);
unsigned long counter = 0;

void setup() {
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,INSTANCE,NETWORKID);
  radio.setHighPower();
  radio.setPowerLevel(0);

  radio.sleep();

  char buff[50];
  sprintf(buff, "\nSending at %d Mhz...", 433);
  Serial.println(buff);
  Serial.flush();
}

void loop() {
  long start = micros();

  // radio.send(2, "Here is some longer test data", 29, false);
  if(radio.sendWithRetry(RECEIVER_ID, &counter, sizeof(counter), 2, 10)) {
    long duration = micros() - start;
    Serial.print(radio.RSSI);
    Serial.print(" ");
    Serial.println(duration);
  } else {
    Serial.println("failed");
  }

  counter++;
  radio.sleep();
  Serial.flush();

  powerDown(WAKEUP_DELAY_1_S);
}
