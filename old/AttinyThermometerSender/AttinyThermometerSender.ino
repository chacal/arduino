#include <VCC.h>
#include <AttinySleepy.h>
#include <RH_ASK.h>
#include <Thermometer.h>

/*
 * Wiring for thermistor:
 * - 10k resistor between GND and D3
 * - thermistor between D3 and 5V
 * 
 * Wiring for radio transmitter:
 * - 5V -> VCC
 * - GND -> GND
 * - DATA -> D4
 */


#define ThermistorPIN 3               // Attiny Pin D3 (physical pin 2)
int dividerResistor = 10000;          // balance/pad resistor value
int calibration = -1100;              // Calibration value, added to pad resistance

#define SLEEP_MS_BETWEEN_MEASUREMENTS 15000 
ISR(WDT_vect) { AttinySleepy::watchdogEvent(); }

#define RF_RX_PIN 0
#define RF_TX_PIN 4
#define RF_PTT_PIN 1
RH_ASK driver(2000, RF_RX_PIN, RF_TX_PIN, RF_PTT_PIN);

struct {
  char tag;
  uint8_t instance;
  float temp;
  int vcc;
} measurements;

void setup() {
  measurements.tag = 't';
  measurements.instance = 1;
  driver.init();
}

void loop() {
  measurements.temp = getTemperature(analogRead(ThermistorPIN), dividerResistor, calibration);
  measurements.vcc = readVcc();

  driver.send((uint8_t*)&measurements, sizeof(measurements));
  driver.waitPacketSent();

  AttinySleepy::loseSomeTime(SLEEP_MS_BETWEEN_MEASUREMENTS);
}

