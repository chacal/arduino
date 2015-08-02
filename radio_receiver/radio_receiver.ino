#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

/*
 * Wiring for RF receiver module (XY-MK-5V):
 * - RF GND -> Arduino GND
 * - RF VCC -> Arduino 5V
 * - RF Data -> Arduino Digital Pin 11
 */

RH_ASK driver;
void setup()
{
    pinMode(13, OUTPUT);
    Serial.begin(9600); // Debugging only
    if (!driver.init())
         Serial.println("init failed");
}
void loop()
{
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);
    if (driver.recv(buf, &buflen)) // Non-blocking
    {
        turnLedOn();
        printMessageFromBuf(buf, buflen);
        turnLedOff();
    }
}

void printMessageFromBuf(uint8_t* src, uint8_t srcLen)
{
    char message[srcLen + 1];
    strFromBuf(message, src, srcLen);
    printWithMillis(message);
}

void strFromBuf(char* dst, uint8_t* src, uint8_t bufLen)
{
  strncpy(dst, (char*)src, bufLen);
  dst[bufLen] = 0;
}

void turnLedOn()
{
    digitalWrite(13, HIGH);
}

void turnLedOff()
{
    digitalWrite(13, LOW);
}

void printWithMillis(char* message)
{
    Serial.print(millis());
    Serial.print(": ");
    Serial.println(message);  
}

