#include "buttons.h"
#include <EnableInterrupt.h>
#include <PushButton.h>

PushButton button1 = PushButton(BUTTON_1_PIN);
PushButton button2 = PushButton(BUTTON_2_PIN);

void setupPinChangeInterrupts() {
  enableInterrupt(BUTTON_1_PIN, []() -> void{}, CHANGE);  // Pass anonymous noop functions as callbacks, because we just want to wake up on pin change
  enableInterrupt(BUTTON_2_PIN, []() -> void{}, CHANGE);
}

void updateButtonStates() {
  button1.update();
  button2.update();
}

bool noButtonsPressed() {
  return !button1.isPressed() && !button2.isPressed();
}

void setupButtonCallbacks() {
  button1.onRelease(0, LONG_PRESS_TIME_MS - 1, onButton1ShortReleased);
  button1.onRelease(LONG_PRESS_TIME_MS, onButton1LongReleased);
}

void onButton1ShortReleased(Button &btn, uint16_t duration) {
  Serial.println("Button 1 was short pressed!");
}

void onButton1LongReleased(Button &btn, uint16_t duration) {
  Serial.println("Button 1 was long pressed!");
}
