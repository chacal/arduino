#include <JeeLib.h>
#include <Thermometer.h>
#include "RF24.h"
#include <EEPROM.h>
#include "main.h"


/*
 * Wiring for thermistor:
 * - 10k resistor between GND and ThermistorIN
 * - thermistor between ThermistorOUT and ThermistorIN
 *
 * Wiring for external voltage measurement:
 * - 1.5 MOhm resistor between RAW and RAW_IN
 * - 1 MOhm resistor between RAW_IN and GND
 */

/* Pins */
#define ThermistorIN A1               // Temperature is measured from this pin
#define ThermistorOUT A0              // This set to HIGH when measuring temperature
#define RAW_IN A4                     // RAW voltage is read from this pin
#define NRF_CE 2                      // Chip Enbale pin for NRF radio
#define NRF_CSN 3                     // SPI Chip Select for NFR radio

#define DIVIDER_RESISTOR 10000        // balance/pad resistor value
#define CALIBRATION -1280             // Calibration value, added to pad resistance

// Measured resistances for the voltage divider resistors
#define R1 1492000
#define R2 982000
#define AREF 3.0  // Regulated 3V used as board's VCC

#define INSTANCE 2

#define CONFIG_EEPROM_ADDR 0
#define FORCE_CONFIG_SAVE 0

RF24 radio(NRF_CE, NRF_CSN);
uint8_t address[6] = "1Node";

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

struct {
  char tag;
  uint8_t instance;
  float temp;
  int vcc;
  unsigned long previousSampleTimeMicros;
} measurements;

struct Config {
  long r1;
  long r2;
  int tempCalibration;
  uint8_t instance;
} config;

void setup() {
  Serial.begin(115200);
  initializeConfig();
  measurements.tag = 't';
  initializeRadio();
  measurements.previousSampleTimeMicros = 0;
}

void loop() {
  radio.powerUp();
  pinMode(ThermistorOUT, OUTPUT);
  digitalWrite(ThermistorOUT, HIGH);
  unsigned long start = micros();

  // Read and wait to get the ADC settled
  analogRead(ThermistorIN);
  delayMicroseconds(100);
  measurements.temp = getTemperature(analogRead(ThermistorIN), DIVIDER_RESISTOR, config.tempCalibration);
  pinMode(ThermistorOUT, INPUT);

  measurements.instance = config.instance;

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

//  Serial.println(micros() - start);
//  Serial.flush();

  Sleepy::loseSomeTime(5000);
}

void initializeConfig() {
  EEPROM.get(CONFIG_EEPROM_ADDR, config);
  if(FORCE_CONFIG_SAVE || (config.r1 == -1 && config.r2 == -1 && config.tempCalibration == -1)) {
    config.r1 = R1;
    config.r2 = R2;
    config.tempCalibration = CALIBRATION;
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
  Serial.flush();
}

void initializeRadio() {
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setPayloadSize(32);
  radio.enableDynamicPayloads();
  radio.openWritingPipe(address);
  radio.setAutoAck(true);
  radio.setRetries(1, 10);  // Retry every 1ms for maximum of 3ms + send times (~1ms)
}
