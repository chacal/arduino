#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/*
 * Wiring:
 * - 5V -> VCC
 * - GND -> GND
 * - A4 -> SDA
 * - A5 -> SCL
 */

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte smiley[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
};

void setup() {
  // initialize the LCD
  lcd.begin();

  lcd.createChar(0, smiley);
  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("Heippa muru!");
  lcd.setCursor(0,1);
  lcd.print("Kuikka!!");
  lcd.write(byte(0));
}

void loop() {
  
}
