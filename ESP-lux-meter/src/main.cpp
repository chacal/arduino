#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_TSL2561_Unified tsl     = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT);
Adafruit_SSD1306         display = Adafruit_SSD1306(128, 32);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);

  if (!tsl.begin()) {
    Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  tsl.setGain(TSL2561_GAIN_1X);
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  sensors_event_t event;
  tsl.getEvent(&event);

  /* Display the results (light is measured in lux) */
  if (event.light) {
    Serial.print(event.light);
    Serial.println(" lux");
  } else {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    Serial.println("Sensor overload");
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print((int) event.light);
  display.println(" lux");
  display.display();

  delay(250);
}