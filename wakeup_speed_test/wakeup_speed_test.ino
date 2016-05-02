#include <JeeLib.h>
#include <avr/sleep.h>
#include <util/atomic.h>

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

bool mode = true;

void setup() {
  Serial.begin(115200);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
}

void loop() {
  digitalWrite(4, HIGH);
  Serial.println(mode);
  Serial.flush();
  delay(1000);
  digitalWrite(4, LOW);
  attachInterrupt(digitalPinToInterrupt(2), toggleState, LOW);
  goToSleep();
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
  detachInterrupt(digitalPinToInterrupt(2));
}

void toggleState() {
  mode = !mode;
}
