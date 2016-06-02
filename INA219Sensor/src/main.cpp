#include <Arduino.h>
#include <Wire.h>
#include <INA219.h>
#include "main.hpp"
#include <power.h>
#include "RF24.h"
#include <EEPROM.h>
#include <VCC.h>


/* Pins */
#define NRF_CE 2                      // Chip Enbale pin for NRF radio
#define NRF_CSN 3                     // SPI Chip Select for NFR radio

#define AREF 3.00  // Regulated 3V used as board's VCC
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
  powerDown(WAKEUP_DELAY_64_MS);  // It takes ~34 ms to measure shunt with 64 samples
  start = micros();

  measurements.rawMeasurement = ina219.shuntVoltageRaw();
  measurements.shuntVoltageMilliVolts = ina219.shuntVoltage() * 1000;
  measurements.shuntCurrent = ina219.shuntCurrent();
  measurements.vcc = readVcc();
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
  ina219.calibrate(0.0005, 0.05, 15, 50);     // 5mΩ shunt, 40mV max shunt voltage, max bus voltage, max current in shunt
}

void configureINA219(Ina219Mode mode) {
  // 0-16V bus voltage range, ±40mV scale, 12-bit bus ADC, 12-bit + 64 sample avg shunt ADC, measure shunt only triggered / power down
  ina219.configure(INA219::RANGE_16V, INA219::GAIN_1_40MV, INA219::ADC_12BIT, INA219::ADC_64SAMP, (INA219::t_mode)mode);  // Force cast mode as INA219 library doesn't support triggered modes..
}


void initializeConfig() {
  EEPROM.get(CONFIG_EEPROM_ADDR, config);
  if(FORCE_CONFIG_SAVE || config.instance == -1 || config.instance == 255) {
    config.instance = INSTANCE;
    Serial.println("Saving defaults to EEPROM...");
    EEPROM.put(CONFIG_EEPROM_ADDR, config);
  }
  Serial.println("Used configuration:");
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
