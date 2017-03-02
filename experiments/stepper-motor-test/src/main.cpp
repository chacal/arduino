#include <Arduino.h>
#include <CheapStepper.h>
#include <PushButton.h>

CheapStepper stepper(8,9,10,11);
PushButton btnFwd = PushButton(2);
PushButton btnBack = PushButton(3);

int degrees = 15;
#define IN true
#define OUT false

void btnInPressed(Button& btn);
void btnInHeld(Button& btn, uint16_t duration, uint16_t repeat_count);
void btnOutPressed(Button& btn);
void btnOutHeld(Button& btn, uint16_t duration, uint16_t repeat_count);

void setup() {
  stepper.setRpm(10);

  Serial.begin(57600);
  Serial.println("Ready to start moving!");
  Serial.println(stepper.getDelay());
  Serial.println(stepper.getRpm());

  btnFwd.onPress(btnInPressed);
  btnFwd.onHoldRepeat(200, 200, btnInHeld);
  btnBack.onPress(btnOutPressed);
  btnBack.onHoldRepeat(200, 200, btnOutHeld);
}

void loop() {
  stepper.run();
  btnFwd.update();
  btnBack.update();
}

void btnInPressed(Button& btn){
  Serial.println("In!");
  stepper.newMoveDegrees(IN, degrees);
}

void btnOutPressed(Button& btn){
  Serial.println("Out!");
  stepper.newMoveDegrees(OUT, degrees);
}

void btnInHeld(Button& btn, uint16_t duration, uint16_t repeat_count) {
  stepper.newMoveDegrees(IN, degrees);
}

void btnOutHeld(Button& btn, uint16_t duration, uint16_t repeat_count) {
  stepper.newMoveDegrees(OUT, degrees);
}
