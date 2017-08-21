#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#include <Button.h>
#include <ButtonEventCallback.h>
#include <PushButton.h>


// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(10, 2, 3);
uint8_t contrast = 90;

PushButton btnDecreaseContrast = PushButton(8);
PushButton btnIncreaseContrast = PushButton(9);


void onDecreaseContrastButton(Button& btn, uint16_t duration) {
  display.setContrast(--contrast);
  Serial.println(contrast);
}

void onIncreaseContrastButton(Button& btn, uint16_t duration) {
  display.setContrast(++contrast);
  Serial.println(contrast);
}

void setup() {
  Serial.begin(57600);
  display.begin(contrast, 0x00);
//  display.setContrast(contrast);

  btnDecreaseContrast.onRelease(onDecreaseContrastButton);
  btnIncreaseContrast.onRelease(onIncreaseContrastButton);

/*
  display.clearDisplay();
  display.fillRect(0,0,84,48,BLACK);
  display.display();
*/

}


void loop() {
//  btnIncreaseContrast.update();
//  btnDecreaseContrast.update();

  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(BLACK);
  display.setCursor(8,5);
  display.print(random(0, 9));
  display.print(".");
  display.println(random(0, 9));

  display.setCursor(34,38);
  display.setTextSize(1);
  display.println("TWS");

  display.display();

  delay(1000);
}