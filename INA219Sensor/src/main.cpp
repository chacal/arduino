#include <Arduino.h>
#include <Wire.h>
#include <INA219.h>
#include "main.hpp"
#include <power.h>
#include "RF24.h"
#include <EEPROM.h>


/* Pins */
#define NRF_CE 6                      // Chip Enbale pin for NRF radio
#define NRF_CSN 8                     // SPI Chip Select for NFR radio
#define EXT_VOLTAGE_SENSE A3

// Measured resistances for the voltage divider resistors
#define R1 39100
#define R2 9900
#define AREF 3.27  // Regulated 3V used as board's VCC

#define INSTANCE 6

#define CONFIG_EEPROM_ADDR 0
#define FORCE_CONFIG_SAVE 0


INA219 ina219;
RF24 radio(NRF_CE, NRF_CSN);
uint8_t address[6] = "1Node";

struct {
  char tag;
  uint8_t instance;
  int16_t rawMeasurement;
  float shuntVoltageMilliVolts;
  float shuntCurrent;
  int vcc;
  unsigned long previousSampleTimeMicros;
} measurements;

struct Config {
  long r1;
  long r2;
  uint8_t instance;
} config;



void setup()
{
  Serial.begin(57600);
  initializeINA219();
  initializeConfig();
  initializeRadio();
  measurements.tag = 'c';
  measurements.previousSampleTimeMicros = 0;
}

void loop()
{
  unsigned long start = micros();
  unsigned long duration = 0;

  radio.powerUp();
  configureINA219(TRIGGER_SHUNT);

  duration += micros() - start;
  powerDown(WAKEUP_DELAY_64_MS, WAKEUP_DELAY_16_MS);  // It takes ~68 ms to measure shunt with 128 samples => sleep for 82ms
  start = micros();

  measurements.rawMeasurement = ina219.shuntVoltageRaw();
  measurements.shuntVoltageMilliVolts = ina219.shuntVoltage() * 1000;
  measurements.shuntCurrent = ina219.shuntCurrent();
  measurements.vcc = readExternalVoltage();
  measurements.instance = config.instance;

  configureINA219(POWER_DOWN);
  radio.writeFast(&measurements, sizeof(measurements));
  radio.txStandBy();
  radio.powerDown();

  measurements.previousSampleTimeMicros = duration + (micros() - start);

  // Serial.print("raw shunt voltage: ");
  // Serial.println(ina219.shuntVoltageRaw());
  // Serial.println(measurements.previousSampleTimeMicros);
  // Serial.flush();

  powerDown(WAKEUP_DELAY_2_S);
}


void initializeINA219() {
  ina219.begin();
  configureINA219(TRIGGER_SHUNT);
  ina219.calibrate(0.00025, 0.05, 15, 200);     // 0.25mΩ shunt, 50mV max shunt voltage, max bus voltage, max current in shunt
}

void configureINA219(Ina219Mode mode) {
  // 0-16V bus voltage range, ±40mV scale, 12-bit bus ADC, 12-bit + 128 sample avg shunt ADC, measure shunt only triggered / power down
  ina219.configure(INA219::RANGE_16V, INA219::GAIN_1_40MV, INA219::ADC_12BIT, INA219::ADC_128SAMP, (INA219::t_mode)mode);  // Force cast mode as INA219 library doesn't support triggered modes..
}

int readExternalVoltage() {
  // Read and wait to get the ADC settled
  analogRead(EXT_VOLTAGE_SENSE);
  delayMicroseconds(100);
  int sensorValue = analogRead(EXT_VOLTAGE_SENSE);
  float voltageAtPin = sensorValue * (AREF / 1023.0);
  int externalVoltageInMilliVolts = (config.r1 + config.r2) / (float)config.r2 * voltageAtPin * 1000;
  return externalVoltageInMilliVolts;
}

void initializeConfig() {
  EEPROM.get(CONFIG_EEPROM_ADDR, config);
  if(FORCE_CONFIG_SAVE || config.instance == -1 || config.instance == 255 || (config.r1 == -1 && config.r2 == -1)) {
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
  radio.setDataRate(RF24_250KBPS);
  radio.setPayloadSize(32);
  radio.enableDynamicPayloads();
  radio.openWritingPipe(address);
  // Use auto ACKs to avoid sleeping between radio transmissions
  radio.setAutoAck(true);
  radio.setRetries(3, 2);  // Retry every 1ms for maximum of 3ms + send times (~1ms)
}
