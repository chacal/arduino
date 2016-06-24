#include "RF24.h"
#include <EEPROM.h>
#include "main.h"
#include <power.h>


/*
 * Wiring for water tank resistor:
 * - 0-180 ohm resistive water tank sensor between GND and WaterTankIN
 * - 470 ohm resistor between WaterTankOUT and WaterTankIN
 *
 * Wiring for external voltage measurement:
 * - 1.5 MOhm resistor between RAW and RAW_IN
 * - 1 MOhm resistor between RAW_IN and GND
 */

/* Pins */
#define WaterTankIN A7                // Water tank level is measured from this pin
#define WaterTankOUT 10               // This set to HIGH when measuring water tank level
#define RAW_IN A6                     // RAW voltage is read from this pin
#define NRF_CE 2                      // Chip Enbale pin for NRF radio
#define NRF_CSN 3                     // SPI Chip Select for NFR radio

#define WATER_MEASUREMENT_DIVIDER_RESISTOR 472          // balance/pad resistor value
#define WATER_TANK_MAX_RESISTOR 180                     // Resistance of full tank meter
#define WATER_TANK_ADC_FULL_VALUE (float)((WATER_TANK_MAX_RESISTOR / ((float)WATER_TANK_MAX_RESISTOR + WATER_MEASUREMENT_DIVIDER_RESISTOR)) * 1023)

// Measured resistances for the voltage divider resistors
#define R1 1468000
#define R2 1000000
#define AREF 3.0  // Regulated 3V used as board's VCC

#define INSTANCE 7

#define CONFIG_EEPROM_ADDR 0
#define FORCE_CONFIG_SAVE 0

RF24 radio(NRF_CE, NRF_CSN);
uint8_t address[6] = "1Node";

struct {
  char tag;
  uint8_t instance;
  uint8_t waterLevelPercentage;
  int vcc;
  unsigned long previousSampleTimeMicros;
} measurements;

struct Config {
  long r1;
  long r2;
  uint8_t instance;
} config;

void setup() {
  Serial.begin(57600);
  initializeConfig();
  measurements.tag = 'w';
  initializeRadio();
  measurements.previousSampleTimeMicros = 0;
  measurements.instance = config.instance;
  pinMode(WaterTankOUT, OUTPUT);
}

void loop() {
  unsigned long start = micros();
  radio.powerUp();

  digitalWrite(WaterTankOUT, HIGH);
  // Read and wait to get the ADC settled
  analogRead(WaterTankIN);
  delayMicroseconds(100);
  float adcValue = min(analogRead(WaterTankIN), WATER_TANK_ADC_FULL_VALUE);  // Cap ADC readings to max tank value => tank level never goes above 100%
  digitalWrite(WaterTankOUT, LOW);
  measurements.waterLevelPercentage = (uint8_t)(100 * (adcValue / WATER_TANK_ADC_FULL_VALUE));

  // Read and wait to get the ADC settled
  analogRead(RAW_IN);
  delayMicroseconds(100);
  int sensorValue = analogRead(RAW_IN);
  float voltageAtPin = sensorValue * (AREF / 1023.0);
  int externalVoltageInMilliVolts = (config.r1 + config.r2) / (float)config.r2 * voltageAtPin * 1000;
  measurements.vcc = externalVoltageInMilliVolts;

  radio.write(&measurements, sizeof(measurements));
  radio.powerDown();

  measurements.previousSampleTimeMicros = micros() - start;

  powerDown(WAKEUP_DELAY_8_S, WAKEUP_DELAY_2_S);
}

void initializeConfig() {
  EEPROM.get(CONFIG_EEPROM_ADDR, config);
  if(FORCE_CONFIG_SAVE || (config.r1 == -1 && config.r2 == -1)) {
    config.r1 = R1;
    config.r2 = R2;
    config.instance = INSTANCE;
    Serial.println("Saving defaults to EEPROM...");
    EEPROM.put(CONFIG_EEPROM_ADDR, config);
  }
  Serial.println("Used configuration:");
  Serial.print("R1 = ");
  Serial.println(config.r1);
  Serial.print("R2 = ");
  Serial.println(config.r2);
  Serial.print("Instance = ");
  Serial.println(config.instance);
  Serial.flush();
}

void initializeRadio() {
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.setPayloadSize(32);
  radio.enableDynamicPayloads();
  radio.openWritingPipe(address);
  radio.setAutoAck(true);
  radio.setRetries(0, 10);  // Retry every 250us for 10 times + send times (~1ms)
}
