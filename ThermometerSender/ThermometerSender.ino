#include <math.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <JeeLib.h>


/*
 * Wiring for thermistor:
 * - 10k resistor between GND and A3
 * - thermistor between A3 and 5V
 * 
 * Wiring for LCD display:
 * - 5V -> VCC
 * - GND -> GND
 * - A4 -> SDA
 * - A5 -> SCL
 */


#define ThermistorPIN A3                // Analog Pin 3
float pad = 9950;                       // balance/pad resistor value
float thermr = 10000;                   // thermistor nominal resistance
float calibration = -1200;               // Calibration value, added to pad resistance

LiquidCrystal_I2C lcd(0x27, 16, 2);

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup() {
  Serial.begin(115200);
  lcd.begin();
}

void loop() {
  float temp = getTemperature(analogRead(ThermistorPIN));
  Serial.print("Celsius: "); 
  Serial.println(temp, 1);
  lcd.clear();
  lcd.print("Temp: ");
  lcd.print(temp, 1);
  lcd.print(" C");
  lcd.setCursor(0,1);
  lcd.print("Voltage: ");
  long vcc = readVcc();
  int voltage1 = vcc / 1000;
  int voltage2 = (((float)vcc / 1000) - voltage1) * 1000;
  char voltageStr[7];
  sprintf(voltageStr, "%d.%d V", voltage1, voltage2);
  lcd.print(voltageStr);

  Sleepy::loseSomeTime(5000);
}

float getTemperature(int RawADC) {
  long resistance = (pad + calibration) * ((1024.0 / RawADC) - 1); 
  float temp = log(resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
  temp = 1 / (0.001129148 + (0.000234125 * temp) + (0.0000000876741 * temp * temp * temp));
  temp = temp - 273.15;  // Convert Kelvin to Celsius                      
  return temp;
}

long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

