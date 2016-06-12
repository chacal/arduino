#ifndef PINCHANGEINTTEST_BUTTONS_H
#define PINCHANGEINTTEST_BUTTONS_H

#include <Button.h>
#include "RF24.h"

#define LONG_PRESS_TIME_MS 1000

#define BUTTON_ENABLE_PIN A1
#define BUTTON_DISABLE_PIN A5
#define BUTTON_PLUS10_PIN 8
#define BUTTON_PLUS1_PIN 5
#define BUTTON_MINUS1_PIN 3
#define BUTTON_MINUS10_PIN 2

void setupPinChangeInterrupts();
void setupButtonCallbacks(RF24 *radio);
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

#endif //PINCHANGEINTTEST_BUTTONS_H
