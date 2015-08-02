#include <CapacitiveSensor.h>
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
#include <JeeLib.h>

/*
 * Connections for 433MHz radio transmitter:
 *  - Arduino 5V -> Tx VCC
 *  - Arduino GND -> Tx GND
 *  - Arduino D7 -> Tx DATA
 *  
 * Connections for capacitive sensing:
 *  - Big resistor between D2 & D3 (> 1 MOhm seems to work)
 *  - Sensor's first wire (e.g. from aluminium foil) to D3
 *  - Sensor's other wire to GND
 */


CapacitiveSensor cs = CapacitiveSensor(2,3);        // 10 megohm resistor between pins D2 & D3. D3 is the sensor pin.
RH_ASK driver(2000, 8, 7, 6, false);
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup()                    
{
   cs.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate
   Serial.begin(9600);
   if (!driver.init())
     Serial.println("init failed");
   pinMode(13, OUTPUT);
}

void loop()                    
{
    digitalWrite(13, HIGH);   // turn the built-in LED on
    long start = millis();
    long total1 =  cs.capacitiveSensor(100);
    long total2 =  cs.capacitiveSensorRaw(100);

    Serial.print(millis() - start);        // check on performance in milliseconds
    Serial.print(": ");
    Serial.print(total1);                  // print sensor output 1
    Serial.print("\t\t");
    Serial.println(total2);

    char buffer[30];
    sprintf(buffer, "val: %ld raw: %ld", total1, total2);
    driver.send((uint8_t*)buffer, strlen(buffer));
    driver.waitPacketSent();

    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
    
    Sleepy::loseSomeTime(10000);
}
