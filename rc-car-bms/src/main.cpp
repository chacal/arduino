#include <Arduino.h>
#include "power.h"
#include "VCC.h"
#include "moving-average.hpp"

#define SHUTDOWN_VOLTAGE_MV         7400
#define EMERGENCY_VOLTAGE_MV        6500
#define MIN_MEASURED_VOLTAGE_MV     1000
#define FET_GATE_PIN                 PB4
#define BAT_MEAS_PIN                  A3
#define AVG_WINDOW_SIZE               80
#define SHUTDOWN_SAFETY_COUNTER       80

//#define PRINT_DEBUG


int readBatVoltage() {
  return readRawVccMilliVolts(BAT_MEAS_PIN, 5, 3380, 2000);
}

void setup() {
  pinMode(FET_GATE_PIN, OUTPUT);
  digitalWrite(FET_GATE_PIN, LOW);
  pinMode(BAT_MEAS_PIN, INPUT);
  movingAvg::init(AVG_WINDOW_SIZE);

  if (readBatVoltage() > SHUTDOWN_VOLTAGE_MV) {
    digitalWrite(FET_GATE_PIN, HIGH);
  }

#ifdef PRINT_DEBUG
  Serial.begin(19200);
#endif
}

uint8_t shutDownSeconds = 0;

void loop() {
#ifdef PRINT_DEBUG
  auto start = micros();
#endif

  auto vcc = readBatVoltage();
  auto avg = movingAvg::calculate(vcc);

  auto currentState = digitalRead(FET_GATE_PIN);
  if (currentState == HIGH && avg < SHUTDOWN_VOLTAGE_MV) {
    shutDownSeconds = 0;
    digitalWrite(FET_GATE_PIN, LOW);
  }

  if (currentState == LOW) {
    if (avg > SHUTDOWN_VOLTAGE_MV) {
      shutDownSeconds++;
      if (shutDownSeconds > SHUTDOWN_SAFETY_COUNTER) {
        digitalWrite(FET_GATE_PIN, HIGH);
      }
    } else {
      shutDownSeconds = 0;
    }
  }

  if (avg > MIN_MEASURED_VOLTAGE_MV && avg < EMERGENCY_VOLTAGE_MV) {
    digitalWrite(FET_GATE_PIN, LOW);
    powerDown();
  }

#ifdef PRINT_DEBUG
  auto end = micros() - start;
  Serial.print("VCC: ");
  Serial.print(vcc);
  Serial.print(" AVG: ");
  Serial.print(avg);
  Serial.print(" Took: ");
  Serial.print(end);
  Serial.println(" us");
#endif

  powerDown(WAKEUP_DELAY_250_MS);
}