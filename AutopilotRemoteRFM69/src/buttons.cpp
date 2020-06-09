#include "buttons.h"
#define EI_NOTEXTERNAL 1  // Don't use external interrupts via EnableInterrupt library as RFM69 needs them
#include <EnableInterrupt.h>
#include <PushButton.h>
#include "main.h"

#define BTN_SHORT_CALLBACK(btnId) [](Button &b, uint16_t i) -> void { onButtonShortReleased(btnId); }
#define BTN_LONG_CALLBACK(btnId) [](Button &b, uint16_t i) -> void { onButtonLongReleased(btnId); }

struct {
  char tag;
  uint8_t instance;
  uint8_t btnId;
  bool isLongPress;
  int vcc;
  unsigned long previousSampleTimeMicros;
} measurements;


RFM69 *radio;
Config *config;
PushButton btnEnable = PushButton(BUTTON_ENABLE_PIN);
PushButton btnDisable = PushButton(BUTTON_DISABLE_PIN);
PushButton btnPlus10 = PushButton(BUTTON_PLUS10_PIN);
PushButton btnPlus1 = PushButton(BUTTON_PLUS1_PIN);
PushButton btnMinus1 = PushButton(BUTTON_MINUS1_PIN);
PushButton btnMinus10 = PushButton(BUTTON_MINUS10_PIN);


void setupPinChangeInterrupts() {
  enableInterrupt(BUTTON_ENABLE_PIN, []() -> void{}, CHANGE);  // Pass anonymous noop functions as callbacks, because we just want to wake up on pin change
  enableInterrupt(BUTTON_DISABLE_PIN, []() -> void{}, CHANGE);
  enableInterrupt(BUTTON_PLUS10_PIN, []() -> void{}, CHANGE);
  enableInterrupt(BUTTON_PLUS1_PIN, []() -> void{}, CHANGE);
  enableInterrupt(BUTTON_MINUS1_PIN, []() -> void{}, CHANGE);
  enableInterrupt(BUTTON_MINUS10_PIN, []() -> void{}, CHANGE);
}

void updateButtonStates() {
  btnEnable.update();
  btnDisable.update();
  btnPlus10.update();
  btnPlus1.update();
  btnMinus1.update();
  btnMinus10.update();
}

bool noButtonsPressed() {
  return !btnEnable.isPressed() &&
    !btnDisable.isPressed() &&
    !btnPlus10.isPressed() &&
    !btnPlus1.isPressed() &&
    !btnMinus1.isPressed() &&
    !btnMinus10.isPressed();
}


void setupButtonCallbacks(RFM69 *r, Config *c) {
  radio = r;
  config = c;
  measurements.tag = 'a';
  measurements.instance = INSTANCE;
  measurements.previousSampleTimeMicros = 0;
  btnEnable.onRelease(0, LONG_PRESS_TIME_MS - 1, BTN_SHORT_CALLBACK(BTN_ENABLE));
  btnEnable.onRelease(LONG_PRESS_TIME_MS, BTN_LONG_CALLBACK(BTN_ENABLE));
  btnDisable.onRelease(0, LONG_PRESS_TIME_MS - 1, BTN_SHORT_CALLBACK(BTN_DISABLE));
  btnDisable.onRelease(LONG_PRESS_TIME_MS, BTN_LONG_CALLBACK(BTN_DISABLE));
  btnPlus10.onRelease(0, LONG_PRESS_TIME_MS - 1, BTN_SHORT_CALLBACK(BTN_PLUS10));
  btnPlus10.onRelease(LONG_PRESS_TIME_MS, BTN_LONG_CALLBACK(BTN_PLUS10));
  btnPlus1.onRelease(0, LONG_PRESS_TIME_MS - 1, BTN_SHORT_CALLBACK(BTN_PLUS1));
  btnPlus1.onRelease(LONG_PRESS_TIME_MS, BTN_LONG_CALLBACK(BTN_PLUS1));
  btnMinus1.onRelease(0, LONG_PRESS_TIME_MS - 1, BTN_SHORT_CALLBACK(BTN_MINUS1));
  btnMinus1.onRelease(LONG_PRESS_TIME_MS, BTN_LONG_CALLBACK(BTN_MINUS1));
  btnMinus10.onRelease(0, LONG_PRESS_TIME_MS - 1, BTN_SHORT_CALLBACK(BTN_MINUS10));
  btnMinus10.onRelease(LONG_PRESS_TIME_MS, BTN_LONG_CALLBACK(BTN_MINUS10));
}

void onButtonShortReleased(ButtonID btnId) {
  sendButtonPress(btnId);
}

void onButtonLongReleased(ButtonID btnId) {
  sendButtonPress(btnId, true);
}

void sendButtonPress(ButtonID btnId, bool isLongPress) {
  long start = micros();
  measurements.vcc = readRawVoltage();
  measurements.btnId = btnId;
  measurements.isLongPress = isLongPress;
  radio->sendWithRetry(RFM_RECEIVER_ID, &measurements, sizeof(measurements), 2, 15);
  radio->sleep();
  measurements.previousSampleTimeMicros = micros() - start;

  if(DEBUG) {
    Serial.println("--------------");
    Serial.print("Button ID:    ");
    Serial.println(btnId);
    Serial.print("Long press:   ");
    Serial.println(isLongPress);
    Serial.print("VCC:          ");
    Serial.println(measurements.vcc);
    Serial.print("Sample time:  ");
    Serial.println(measurements.previousSampleTimeMicros);
  }
}

int readRawVoltage() {
  // Read and wait to get the ADC settled
  analogRead(RAW_IN_PIN);
  delayMicroseconds(100);
  int sensorValue = analogRead(RAW_IN_PIN);
  float voltageAtPin = sensorValue * (AREF / 1023.0);
  int externalVoltageInMilliVolts = (config->r1 + config->r2) / (float)config->r2 * voltageAtPin * 1000;
  return externalVoltageInMilliVolts;
}
