#include <CapacitiveSensor.h>
#include <RH_ASK.h>
#include <AttinySleepy.h>

CapacitiveSensor cs = CapacitiveSensor(1,2);        // 10 megohm resistor between pins D2 & D3. D3 is the sensor pin.
RH_ASK driver(2000,4,3,2);
ISR(WDT_vect) { AttinySleepy::watchdogEvent(); }

union {
  long total;
  uint8_t totalBytes[4];
} total;

void setup() {
  cs.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate
  pinMode(0, OUTPUT);
  driver.init();
  digitalWrite(0, LOW);
}

void loop() {
  digitalWrite(0, HIGH);
  //total.total = cs.capacitiveSensorRaw(100);
  long laa = cs.capacitiveSensorRaw(100);
  driver.send((uint8_t*)&laa, 4);
  

//  const char *msg = "hello";
//  driver.send((uint8_t *)msg, strlen(msg));
  driver.waitPacketSent();
  
  digitalWrite(0, LOW);
  AttinySleepy::loseSomeTime(1000);
}
