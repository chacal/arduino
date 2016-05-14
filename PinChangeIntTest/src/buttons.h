#ifndef PINCHANGEINTTEST_BUTTONS_H
#define PINCHANGEINTTEST_BUTTONS_H

#include <Button.h>

#define LONG_PRESS_TIME_MS 1000

#define BUTTON_1_PIN 11
#define BUTTON_2_PIN 12

void setupPinChangeInterrupts();
void setupButtonCallbacks();
void updateButtonStates();
bool noButtonsPressed();

void onButton1ShortReleased(Button &btn, uint16_t duration);
void onButton1LongReleased(Button &btn, uint16_t duration);

#endif //PINCHANGEINTTEST_BUTTONS_H
