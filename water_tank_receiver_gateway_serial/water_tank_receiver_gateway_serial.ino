#include <RH_ASK.h>
#include <SPI.h>  // Needed only for compiling

/*
 * Wiring for RF receiver module (XY-MK-5V):
 * - RF GND -> Arduino GND
 * - RF VCC -> Arduino VCC
 * - RF Data -> Arduino Digital Pin 11
 * 
 * Raspi               Arduino
 * ---------------------------
 * P1-02 (5V0)         RAW
 * P1-06 (GND)         GND
 * P1-8 (UART_TX)      RX1
 * P1-10 (UART_RX)     TX0 
 */
 
RH_ASK driver(2000);
long waterTankLevel = 0;
long vcc = 0;

void setup()
{
  driver.init();
  Serial.begin(115200);
  pinMode(13, OUTPUT);
}

void loop()
{
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);
  if (driver.recv(buf, &buflen)) // Non-blocking
  {
    long* values = (long*)buf;
    waterTankLevel = values[0];
    if(buflen == 8) {
      vcc = values[1];
    }
    digitalWrite(13, HIGH);
    sendData();
    digitalWrite(13, LOW);
  }

  if(Serial.available()) {
    char command = Serial.read();
    if(command == '1') {
      sendData();
    }
  }
}

void sendData() 
{
  char buf[200];
  sprintf(buf, "{\"waterTankLevel\": %ld, \"vccmV\": %ld}", waterTankLevel, vcc);
  Serial.println(buf);
}

