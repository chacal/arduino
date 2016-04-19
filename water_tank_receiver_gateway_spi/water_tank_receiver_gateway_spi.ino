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
 * P1-19 (MOSI)        D11
 * P1-21 (MISO)        D12 
 * P1-23 (SCLK)        D13 
 * P1-24 (SS0/CE0)     D10 
 */
 
RH_ASK driver(2000, 9);
long measurements[2] = {0, 0};
long sendBuf[2] = {0, 0};
uint8_t pos = 0;

void setup()
{
    driver.init();
    pinMode(MISO, OUTPUT);
    SPCR |= _BV(SPE);
    SPCR |= _BV(SPIE);
}

void loop()
{

    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);
    if (driver.recv(buf, &buflen)) // Non-blocking
    {
        long* values = (long*)buf;
        measurements[0] = values[0];
        if(buflen == 8) {
          measurements[1] = values[1];
        }
    }
    delay(200);
}

ISR (SPI_STC_vect)
{
  if(SPDR == 'a') {
    sendBuf[0] = measurements[0];
    sendBuf[1] = measurements[1];
    pos = 0;
  }
   
  const byte* p = (const byte*)sendBuf;
  SPDR = *(p + pos);
  pos++;
  if(pos == 8)
    pos = 0;
}

