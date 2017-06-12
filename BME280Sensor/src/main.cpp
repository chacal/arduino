#include "Arduino.h"
#include <BME280I2C.h>
#include <Wire.h>
#include "RF24.h"
#include <EEPROM.h>
#include <VCC.h>
#include "main.h"

/*
 * Wiring for external voltage measurement:
 * - 1.5 MOhm resistor between RAW and RAW_IN
 * - 1 MOhm resistor between RAW_IN and GND
 */

/* Pins */
#define RAW_IN A3                     // RAW voltage is read from this pin
#define NRF_CE 9                      // Chip Enbale pin for NRF radio
#define NRF_CSN 10                    // SPI Chip Select for NFR radio

// Measured resistances for the voltage divider resistors (for calculating VCC)
#define R1 2000000
#define R2 2000000
#define AREF 3.33  // Regulated 3V3 used as board's VCC

#define TEMP_CALIBRATION -1.1f       // Calibration value for temperature
#define PRESSURE_CALIBRATION -1.2f   // Calibration value for pressure

#define INSTANCE 11

#define CONFIG_EEPROM_ADDR 0
#define FORCE_CONFIG_SAVE 0


BME280I2C bme(0x01, 0x01, 0x01, 0x01); // x1 oversampling for all, forced mode

RF24 radio(NRF_CE, NRF_CSN);
uint8_t address[6] = "1Node";

unsigned long duration = 0;
unsigned long start = 0;

struct {
  char tag;
  uint8_t instance;
  float value;
  int vcc;
  unsigned long previousSampleTimeMicros;
} measurements;

struct Config {
  long r1;
  long r2;
  float tempCalibration;
  float pressureCalibration;
  uint8_t instance;
} config;



void setup() {
  Serial.begin(57600);
  bme.begin();
  initializeConfig();
  initializeRadio();
  measurements.previousSampleTimeMicros = 0;
}


void loop() {
  duration = 0;

  radio.powerUp();

  float temperature, pressure, humidity;
  measureAndRead(temperature, pressure, humidity, config.tempCalibration, config.pressureCalibration);
  measurements.vcc = readRawVccMilliVolts(RAW_IN, AREF, config.r1, config.r2);
  measurements.instance = config.instance;

  measurements.tag = 't';
  measurements.value = temperature;
  radio.writeFast(&measurements, sizeof(measurements));

  measurements.tag = 'p';
  measurements.value = pressure;
  radio.writeFast(&measurements, sizeof(measurements));

  measurements.tag = 'h';
  measurements.value = humidity;
  radio.writeFast(&measurements, sizeof(measurements));

  radio.txStandBy();
  radio.powerDown();

  measurements.previousSampleTimeMicros = duration + (micros() - start);

  measureTimeAndSleep(WAKEUP_DELAY_8_S, WAKEUP_DELAY_8_S);
}

void measureAndRead(float &temp, float &pressure, float &humidity, float tempCalibration, float pressureCalibration) {
  bme.setMode(0x01);
  measureTimeAndSleep(WAKEUP_DELAY_16_MS);
  bme.read(pressure, temp, humidity, true, 1);  // use metric units & hPa as pressure units
  temp = temp + tempCalibration;
  pressure = pressure + pressureCalibration;
}

void measureTimeAndSleep(WatchdogTimerPrescaler delay1, WatchdogTimerPrescaler delay2) {
  duration += micros() - start;
  powerDown(delay1, delay2);
  start = micros();
}

void initializeConfig() {
  EEPROM.get(CONFIG_EEPROM_ADDR, config);
  if(FORCE_CONFIG_SAVE || (config.r1 == -1 || config.r2 == -1 || config.tempCalibration == -1 || config.pressureCalibration == -1)) {
    config.r1 = R1;
    config.r2 = R2;
    config.tempCalibration = TEMP_CALIBRATION;
    config.pressureCalibration = PRESSURE_CALIBRATION;
    config.instance = INSTANCE;
    Serial.println("Saving defaults to EEPROM...");
    EEPROM.put(CONFIG_EEPROM_ADDR, config);
  }
  Serial.println("Used configuration:");
  Serial.print("R1 = ");
  Serial.println(config.r1);
  Serial.print("R2 = ");
  Serial.println(config.r2);
  Serial.print("Temp calibration = ");
  Serial.println(config.tempCalibration);
  Serial.print("Pressure calibration = ");
  Serial.println(config.pressureCalibration);
  Serial.print("Instance = ");
  Serial.println(config.instance);
  Serial.flush();
}

void initializeRadio() {
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setPayloadSize(32);
  radio.enableDynamicPayloads();
  radio.openWritingPipe(address);
  // Use auto ACKs to avoid sleeping between radio transmissions
  radio.setAutoAck(true);
  radio.setRetries(1, 10);
}
