#include "RF24.h"
#include <EEPROM.h>
#include "main.h"
#include "math.h"

/*
 * Wiring for NTC 100k thermistor:
 * - 100k resistor between GND and ThermistorIN
 * - thermistor between ThermistorOUT and ThermistorIN
 *
 * Wiring for external voltage measurement:
 * - 2 MOhm resistor between RAW and RAW_IN
 * - 2 MOhm resistor between RAW_IN and GND
 */

/* Pins */
#define ThermistorIN A4                 // Temperature is measured from this pin
#define ThermistorOUT A5                // This set to HIGH when measuring temperature
#define RAW_IN A3                       // RAW voltage is read from this pin
#define NRF_CE 9                        // Chip Enbale pin for NRF radio
#define NRF_CSN 10                      // SPI Chip Select for NFR radio

#define TEMP_CALIBRATION 0              // Calibration value, added to pad resistance
#define THERMISTOR_RESISTANCE 100000.0  // Both resistors of the thermistor voltage divired are assumed to be the same
#define THERMISTOR_BETA_COEF 3950       // From thermistor's datasheet
#define THERMISTOR_NOMINAL_TEMP 25      // From thermistor's datasheet (25°C)

// Measured resistances for the voltage divider resistors
#define R1 2000000
#define R2 2000000
#define AREF 3.33  // Regulated 3V used as board's VCC

#define INSTANCE 9

#define CONFIG_EEPROM_ADDR 0
#define FORCE_CONFIG_SAVE 0

RF24 radio(NRF_CE, NRF_CSN);
uint8_t address[6] = "1Node";

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
  Serial.begin(57600);
  initializeConfig();
  measurements.tag = 't';
  initializeRadio();
  measurements.previousSampleTimeMicros = 0;
}

void loop() {
  unsigned long start = micros();

  radio.powerUp();
  pinMode(ThermistorOUT, OUTPUT);
  digitalWrite(ThermistorOUT, HIGH);

  float adcAverage = sampleAdc(ThermistorIN, 5);
  measurements.temp = calculateTemperature(adcAverage);

  pinMode(ThermistorOUT, INPUT);

  measurements.instance = config.instance;
  measurements.vcc = readRawVcc();

  radio.write(&measurements, sizeof(measurements));
  radio.powerDown();

  measurements.previousSampleTimeMicros = micros() - start;

  powerDown(WAKEUP_DELAY_8_S, WAKEUP_DELAY_8_S);
}

float sampleAdc(uint8_t adcPin, uint8_t sampleCount) {
  float average = 0;
  // take N samples in a row, with a slight delay
  for (uint8_t i = 0; i < sampleCount; i++) {
    uint16_t sample = analogRead(adcPin);
    average += sample;
    delayMicroseconds(100);
  }
  average /= sampleCount;
  return average;
}

double calculateTemperature(int rawADC) {
  float steinhart;
  steinhart = rawADC / THERMISTOR_RESISTANCE;             // (R/Ro)
  steinhart = log(steinhart);                             // ln(R/Ro)
  steinhart /= THERMISTOR_BETA_COEF;                      // 1/B * ln(R/Ro)
  steinhart += 1.0 / (THERMISTOR_NOMINAL_TEMP + 273.15);  // + (1/To)
  steinhart = 1.0 / steinhart;                            // Invert
  steinhart -= 273.15;                                    // convert to C
  return steinhart / 10;
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
