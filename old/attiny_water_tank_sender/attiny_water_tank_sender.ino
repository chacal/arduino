#include <CapacitiveSensor.h>
#include <RH_ASK.h>
#include <AttinySleepy.h>
#include <VCC.h>

#define SENSOR_PIN_1 0  // 10 megohm resistor between pins D0 & D1. D1 is the sensor pin.
#define SENSOR_PIN_2 1
#define RF_RX_PIN 4
#define RF_TX_PIN 2  // D2 is RF TX pin, D0 is used for PTT (it is not needed for capacitive sensing when radio is active)
#define RF_PTT_PIN 0
#define CAPACITIVE_ITERATIONS 30
#define SLEEP_MS_BETWEEN_MEASUREMENTS 15000

CapacitiveSensor cs = CapacitiveSensor(SENSOR_PIN_1, SENSOR_PIN_2);        
RH_ASK driver(2000, RF_RX_PIN, RF_TX_PIN, RF_PTT_PIN);  
ISR(WDT_vect) { AttinySleepy::watchdogEvent(); }

struct {
  char tag;
  uint8_t instance;
  long sensorValue;
  int vcc;
} measurements;

void setup() {
  measurements.tag = 'w';
  measurements.instance = 1;
  cs.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate
  driver.init();
}

void loop() {  
  measurements.sensorValue = cs.capacitiveSensorRaw(CAPACITIVE_ITERATIONS);
  measurements.vcc = readVcc();

  driver.send((uint8_t*)&measurements, sizeof(measurements));
  driver.waitPacketSent();
  
  AttinySleepy::loseSomeTime(SLEEP_MS_BETWEEN_MEASUREMENTS);
}

