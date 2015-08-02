#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
#include <JeeLib.h>

/*
 * Connections:
 *  - Arduino 5V -> Tx VCC
 *  - Arduino GND -> Tx GND
 *  - Arduino D7 -> Tx DATA
 */

RH_ASK driver(2000, 8, 7, 6, false);
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup()
{
    pinMode(13, OUTPUT);
    Serial.begin(9600);   // Debugging only
    if (!driver.init())
         Serial.println("init failed");
}
void loop()
{
    digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
    const char *msg = "Hello world! This is a very very";
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
    Sleepy::loseSomeTime(2000);
}
