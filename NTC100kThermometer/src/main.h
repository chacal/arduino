#include "Arduino.h"
#include <power.h>

void initializeConfig();
void initializeRadio();
float sampleAdc(uint8_t adcPin, uint8_t sampleCount);
double calculateTemperature(int rawADC);
