#include "Arduino.h"
#include <SFE_BMP180.h>
#include <Wire.h>
#include "RF24.h"
#include <EEPROM.h>
#include "main.h"

/*
 * Wiring for external voltage measurement:
 * - 1.5 MOhm resistor between RAW and RAW_IN
 * - 1 MOhm resistor between RAW_IN and GND
 */

/* Pins */
#define RAW_IN A0                     // RAW voltage is read from this pin
#define NRF_CE 2                      // Chip Enbale pin for NRF radio
#define NRF_CSN 3                     // SPI Chip Select for NFR radio

// Measured resistances for the voltage divider resistors (for calculating VCC)
#define R1 1520000
#define R2 1001000
#define AREF 3.00  // Regulated 3V used as board's VCC

#define TEMP_CALIBRATION -0.0f  // Calibration value for temperature

#define INSTANCE 5

#define CONFIG_EEPROM_ADDR 0
#define FORCE_CONFIG_SAVE 0


SFE_BMP180 bmp180;
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
  uint8_t instance;
} config;



void setup() {
  Serial.begin(57600);
  bmp180.begin();
  initializeConfig();
  initializeRadio();
  measurements.previousSampleTimeMicros = 0;
}


void loop() {
  duration = 0;

  radio.powerUp();

  double temperature, pressure;
  readTempAndPressure(temperature, pressure, config.tempCalibration);
  measurements.vcc = readRawVcc();
  measurements.instance = config.instance;

  measurements.tag = 't';
  measurements.value = temperature;
  radio.writeFast(&measurements, sizeof(measurements));

  measurements.tag = 'p';
  measurements.value = pressure;
  radio.writeFast(&measurements, sizeof(measurements));
  radio.txStandBy();

  radio.powerDown();

  measurements.previousSampleTimeMicros = duration + (micros() - start);

  measureTimeAndSleep(WAKEUP_DELAY_4_S, WAKEUP_DELAY_1_S);
}

void readTempAndPressure(double &temp, double &pressure, float tempCalibration) {
  bmp180.startTemperature();
  measureTimeAndSleep(WAKEUP_DELAY_16_MS);
  bmp180.getTemperature(temp);
  bmp180.startPressure(0);
  measureTimeAndSleep(WAKEUP_DELAY_16_MS);
  bmp180.getPressure(pressure, temp);
  temp = temp + tempCalibration;
}

void measureTimeAndSleep(WatchdogTimerPrescaler delay1, WatchdogTimerPrescaler delay2) {
  duration += micros() - start;
  powerDown(delay1, delay2);
  start = micros();
}

int readRawVcc() {
  // Read and wait to get the ADC settled
  analogRead(RAW_IN);
  delayMicroseconds(100);
  int sensorValue = analogRead(RAW_IN);
  float voltageAtPin = sensorValue * (AREF / 1023.0);
  int externalVoltageInMilliVolts = (config.r1 + config.r2) / (float)config.r2 * voltageAtPin * 1000;
  return externalVoltageInMilliVolts;
}

void initializeConfig() {
  EEPROM.get(CONFIG_EEPROM_ADDR, config);
  if(FORCE_CONFIG_SAVE || (config.r1 == -1 || config.r2 == -1 || config.tempCalibration == -1)) {
    config.r1 = R1;
    config.r2 = R2;
    config.tempCalibration = TEMP_CALIBRATION;
    config.instance = INSTANCE;
    Serial.println("Saving defaults to EEPROM...");
    EEPROM.put(CONFIG_EEPROM_ADDR, config);
  }
  Serial.println("Used configuration:");
  Serial.print("R1 = ");
  Serial.println(config.r1);
  Serial.print("R2 = ");
  Serial.println(config.r2);
  Serial.print("Calibration = ");
  Serial.println(config.tempCalibration);
  Serial.print("Instance = ");
  Serial.println(config.instance);
}

void initializeRadio() {
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.setPayloadSize(32);
  radio.enableDynamicPayloads();
  radio.openWritingPipe(address);
  // Use auto ACKs to avoid sleeping between radio transmissions
  radio.setAutoAck(true);
  radio.setRetries(0, 10);  // Retry every 1ms for maximum of 3ms + send times (~1ms)
}
