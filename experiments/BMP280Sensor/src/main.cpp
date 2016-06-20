#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp280;

void setup() {
  Serial.begin(57600);
  if(! bmp280.begin(0x76)) {
    Serial.println("Failed to initialize sensor.");
    while (1);
  }
}


void loop() {
  Serial.print(F("Temperature = "));
  Serial.print(bmp280.readTemperature());
  Serial.println(" *C");

  Serial.print(F("Pressure = "));
  Serial.print(bmp280.readPressure());
  Serial.println(" Pa");

  delay(5000);
}
