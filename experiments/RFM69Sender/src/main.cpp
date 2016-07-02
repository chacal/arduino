#include <Arduino.h>
#include <RFM69.h>    //get it here: https://www.github.com/lowpowerlab/rfm69
#include <SPI.h>


#define NETWORKID     50  //the same on all nodes that talk to each other
#define INSTANCE      2
#define RECEIVER_ID   1  // Gateway is ID 1
#define RFM69_NSS_PIN 8
#define FREQUENCY     RF69_433MHZ
#define SERIAL_BAUD   57600

RFM69 radio(RFM69_NSS_PIN, RF69_IRQ_PIN, true);

int previousRssi;
long previousDuration;

void setup() {
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,INSTANCE,NETWORKID);
  radio.setHighPower();
  radio.setPowerLevel(0);

  Serial.println("Sending at 433 Mhz...");
}

void loop() {
  long start = micros();

  bool ret = radio.sendWithRetry(RECEIVER_ID, &previousDuration, sizeof(previousDuration), 2, 50);
  long duration = micros() - start;
  previousRssi = radio.RSSI;
  previousDuration = duration;

  Serial.print(previousRssi);
  Serial.print(" ");
  Serial.print(duration);
  Serial.println(ret ? " OK" : " failed");


  delay(1000);
}
