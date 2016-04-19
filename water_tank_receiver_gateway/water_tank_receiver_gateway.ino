#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile
#include <Wire.h>


/*
 * Wiring for RF receiver module (XY-MK-5V):
 * - RF GND -> Arduino GND
 * - RF VCC -> Arduino VCC
 * - RF Data -> Arduino Digital Pin 11
 * 
 * Raspi               Arduino
 * ---------------------------
 * P1-02 (5V0)         RAW
 * P1-06 (GND)         GND
 * P1-03 (SDA)         A4
 * P1-05 (SCL)         A5 
 */
 
RH_ASK driver(2000);
uint8_t lastMessage[RH_ASK_MAX_MESSAGE_LEN];
uint8_t lastMessageLen = 0;
uint8_t command = 0;   // 0 -> send last message length, 1 -> send last message content

void setup()
{
    pinMode(13, OUTPUT);
    driver.init();
    Wire.begin(8);                // join i2c bus with address #8
    Wire.onReceive(onI2CCommandReceived);
    Wire.onRequest(onI2CReadRequest);
}

void loop()
{
    uint8_t readAmount = sizeof(lastMessage);
    if (driver.recv(lastMessage, &readAmount)) // Non-blocking
    {
        lastMessageLen = readAmount;
        turnLedOn();
        delay(10);
        turnLedOff();
    }
}

void onI2CCommandReceived(int receivedByteCount) {
  if(receivedByteCount == 1 && Wire.available()) {
    command = Wire.read();
  }
}

void onI2CReadRequest() {
  if(command == 0) {
    Wire.write(lastMessageLen);
  } else if(command == 1) {
    Wire.write(lastMessage, lastMessageLen);
  }
}

void turnLedOn()
{
    digitalWrite(13, HIGH);
}

void turnLedOff()
{
    digitalWrite(13, LOW);
}

