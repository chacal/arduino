#include <Arduino.h>
#include <EnableInterrupt.h>
#include <avr/sleep.h>
#include <util/atomic.h>

#define INT_PIN 12

/*
 * This sketch uses less that 1µA when in sleep, but Arduino is woken up when the button is pressed
 */

int interruptCount = 0;
bool buttonPressed = false;

void interruptFunction();
void goToSleep();

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(115200);
  pinMode(INT_PIN, INPUT_PULLUP);
  enableInterrupt(INT_PIN, interruptFunction, FALLING);
}


void loop() {
  if(buttonPressed) {
    buttonPressed = false;
    Serial.print("Pin was interrupted: ");
    Serial.print(interruptCount, DEC);
    Serial.println(" times so far.");
    Serial.flush();
  }

  goToSleep();
}


void interruptFunction() {
  interruptCount++;
  buttonPressed = true;
}

void goToSleep() {
  ADCSRA &= ~ bit(ADEN); // disable the ADC
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    sleep_enable();
#ifdef BODSE
    MCUCR = MCUCR | bit(BODSE) | bit(BODS); // timed sequence
    MCUCR = (MCUCR & ~ bit(BODSE)) | bit(BODS);
#endif
  }

  sleep_cpu();

  sleep_disable();
}
