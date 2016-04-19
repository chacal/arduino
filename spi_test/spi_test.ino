#include <SPI.h>

volatile uint8_t count = 0;

long values[2];

void setup() {
    values[0] = 12345;
    values[1] = 67890;

    pinMode(MISO, OUTPUT);
    SPCR |= _BV(SPE);
    SPCR |= _BV(SPIE);
}

void loop() {
  delay(300);
}

ISR (SPI_STC_vect)
{
  if(SPDR == 'a') {
    count = 0;
  }

  const byte* p = (const byte*)values;
  SPDR = *(p + count);
  count++;
  if(count == 8)
    count = 0;
}

