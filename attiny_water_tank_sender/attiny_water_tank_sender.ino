#include <CapacitiveSensor.h>
#include <RH_ASK.h>
#include <AttinySleepy.h>
#include <util/delay.h>

#define SENSOR_PIN_1 0  // 10 megohm resistor between pins D0 & D1. D1 is the sensor pin.
#define SENSOR_PIN_2 1
#define LED_PIN 3
#define RF_RX_PIN 4
#define RF_TX_PIN 2  // D2 is RF TX pin, D0 is used for PTT (it is not needed for capacitive sensing when radio is active)
#define RF_PTT_PIN 0
#define CAPACITIVE_ITERATIONS 30

uint8_t vccReportCounter = 0;

CapacitiveSensor cs = CapacitiveSensor(SENSOR_PIN_1, SENSOR_PIN_2);        
RH_ASK driver(2000, RF_RX_PIN, RF_TX_PIN, RF_PTT_PIN);  
ISR(WDT_vect) { AttinySleepy::watchdogEvent(); }

void setup() {
  cs.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate
  pinMode(LED_PIN, OUTPUT);
  driver.init();
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  
  long values[3];
  values[0] = cs.capacitiveSensor(CAPACITIVE_ITERATIONS);
  values[1] = cs.capacitiveSensorRaw(CAPACITIVE_ITERATIONS);
  uint8_t bytesToSend = 8;

  if(++vccReportCounter == 12) {  // Report VCC ~once a minute
    vccReportCounter = 0;
    values[2] = readVcc();
    bytesToSend = 12;
  }

  driver.send((uint8_t*)values, bytesToSend);  
  driver.waitPacketSent();
  
  digitalWrite(LED_PIN, LOW);
  AttinySleepy::loseSomeTime(5000);
}


long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  _delay_ms(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
 
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;  // This might return 0 sometimes?
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}
