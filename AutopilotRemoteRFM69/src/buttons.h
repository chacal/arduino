#ifndef AUTOPILOTREMOTERFM69_BUTTONS_H
#define AUTOPILOTREMOTERFM69_BUTTONS_H

#include <RFM69.h>
#include "main.h"

#define LONG_PRESS_TIME_MS 1000

#define BUTTON_ENABLE_PIN  A5
#define BUTTON_DISABLE_PIN  9
#define BUTTON_PLUS10_PIN  A3
#define BUTTON_PLUS1_PIN   A1
#define BUTTON_MINUS1_PIN  A0
#define BUTTON_MINUS10_PIN 10

#define RAW_IN_PIN         A2

void setupPinChangeInterrupts();
void setupButtonCallbacks(RFM69 *radio, Config *config);
void updateButtonStates();
bool noButtonsPressed();

enum ButtonID {
  BTN_ENABLE = 1,
  BTN_DISABLE = 2,
  BTN_PLUS10 = 3,
  BTN_PLUS1 = 4,
  BTN_MINUS1 = 5,
  BTN_MINUS10 = 6
};

void onButtonShortReleased(ButtonID btnId);
void onButtonLongReleased(ButtonID btnId);

void sendButtonPress(ButtonID btnId, bool isLongPress = false);
int readRawVoltage();

#endif //AUTOPILOTREMOTERFM69_BUTTONS_H
