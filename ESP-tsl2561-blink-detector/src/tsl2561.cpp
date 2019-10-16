#include "tsl2561.hpp"

#include <Arduino.h>
#include <Wire.h>

#define PIN_INT  D5

static volatile bool int_pending = false;

static uint8_t read8(uint8_t reg) {
  uint8_t reg_with_command = reg | TSL2561_COMMAND_BIT;

  Wire.beginTransmission(TSL2561_ADDR_FLOAT);
  Wire.write(reg_with_command);
  Wire.endTransmission();

  Wire.requestFrom(TSL2561_ADDR_FLOAT, 1);
  return Wire.read();
}

static uint16_t read16(uint8_t reg) {
  uint8_t reg_with_command_and_word = reg | TSL2561_COMMAND_BIT | TSL2561_WORD_BIT;

  Wire.beginTransmission(TSL2561_ADDR_FLOAT);
  Wire.write(reg_with_command_and_word);
  Wire.endTransmission();

  uint8_t res[2];
  Wire.requestFrom(TSL2561_ADDR_FLOAT, 2);

  res[0] = Wire.read();
  res[1] = Wire.read();
  return res[0] + 256 * res[1];
}

static void write8(uint8_t reg, uint8_t data) {
  uint8_t reg_with_command = reg | TSL2561_COMMAND_BIT;

  Wire.beginTransmission(TSL2561_ADDR_FLOAT);
  Wire.write(reg_with_command);
  Wire.write(data);
  Wire.endTransmission();
}

static void write16(uint8_t reg, uint16_t data) {
  uint8_t reg_with_command = reg | TSL2561_COMMAND_BIT | TSL2561_WORD_BIT;
  uint8_t data_low         = data & 0xFF;
  uint8_t data_high        = data >> 8;

  Wire.beginTransmission(TSL2561_ADDR_FLOAT);
  Wire.write(reg_with_command);
  Wire.write(data_low);
  Wire.write(data_high);
  Wire.endTransmission();
}

void tsl2561_enable() {
  write8(TSL2561_REGISTER_CONTROL, TSL2561_CONTROL_POWERON);
}

void tsl2561_disable() {
  write8(TSL2561_REGISTER_CONTROL, TSL2561_CONTROL_POWEROFF);
}

void tsl2561_clear_interrupt() {
  int_pending                        = false;
  uint8_t reg_with_command_and_clear = TSL2561_REGISTER_CONTROL | TSL2561_COMMAND_BIT | TSL2561_CLEAR_BIT;

  Wire.beginTransmission(TSL2561_ADDR_FLOAT);
  Wire.write(reg_with_command_and_clear);
  Wire.endTransmission();
}

uint16_t tsl2561_get_adc0_value() {
  return read16(TSL2561_REGISTER_CHAN0_LOW);
}

void tsl2561_set_int_low_threshold(uint16_t threshold) {
  write16(TSL2561_REGISTER_THRESHHOLDL_LOW, threshold);
}

void tsl2561_set_int_high_threshold(uint16_t threshold) {
  write16(TSL2561_REGISTER_THRESHHOLDH_LOW, threshold);
}

static void ICACHE_RAM_ATTR on_interrupt() {
  int_pending = true;
}

static void pin_interrupt_init() {
  pinMode(PIN_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_INT), on_interrupt, FALLING);
}

void tsl2561_init() {
  Serial.println("Initializing TSL2561");
  Wire.setClock(400000);
  Wire.begin();

  tsl2561_enable();
  tsl2561_clear_interrupt();

  pin_interrupt_init();

  write8(TSL2561_REGISTER_TIMING, TSL2561_INTEGRATIONTIME_101MS | TSL2561_GAIN_16X);
  write8(TSL2561_REGISTER_INTERRUPT, 0x10);  // Level interrupts, trigger after every ADC read
}

bool tsl2561_interrupt_pending() {
  return int_pending;
}