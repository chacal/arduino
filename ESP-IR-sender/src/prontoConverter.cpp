#include <Arduino.h>
#include "prontoConverter.h"

bool ishex(char ch) {
  return ((ch >= '0') && (ch <= '9')) ||
         ((ch >= 'A') && (ch <= 'F')) ||
         ((ch >= 'a') && (ch <= 'f'));
}

bool isblank(char ch) {
  return (ch == ' ') || (ch == '\t') || (ch == '\0');
}

void byp(const char **pcp) {
  while(isblank(**pcp)) (*pcp)++;
}

uint8_t htob(char ch) {
  if((ch >= '0') && (ch <= '9')) return ch - '0';
  if((ch >= 'A') && (ch <= 'F')) return ch - 'A' + 10;
  if((ch >= 'a') && (ch <= 'f')) return ch - 'a' + 10;
  return -1;
}

uint16_t htow(const char *cp) {
  return (htob(cp[0]) << 12) | (htob(cp[1]) << 8) | (htob(cp[2]) << 4) | (htob(cp[3]));
}

uint16_t readWordAndSkip(const char **pcp) {
  byp(pcp);
  uint16_t res = htow(*pcp);
  (*pcp) += 4;
  return res;
}

bool isValidProntoString(const char *inputString) {
  const char *cp;
  for(cp = inputString; *cp; cp += 4) {
    byp(&cp);
    if(!ishex(cp[0]) || !ishex(cp[1]) ||
       !ishex(cp[2]) || !ishex(cp[3]) || !isblank(cp[4]))
      return false;
  }

  return true;
}

bool convertProntoToRaw(const char *prontoHex, RawSampleData *sampleData) {
  const char *cp;
  uint16_t freq;  // Frequency in KHz
  uint8_t usec;  // pronto uSec/tick
  uint8_t codeLength;
  uint8_t repeatLength;


  //  Validate input
  cp = prontoHex;

  if(!isValidProntoString(cp) || readWordAndSkip(&cp) != 0000) {
    return false;
  }


  // Read frequency and calculate single pulse length
  freq = (uint16_t) (1000000 / (readWordAndSkip(&cp) * 0.241246));
  usec = (uint8_t) (((1.0 / freq) * 1000000) + 0.5);
  freq /= 1000;
  sampleData->freq = (uint8_t) freq;


  //  Read and validate once and repeat code lengths
  codeLength = (uint8_t) readWordAndSkip(&cp);
  repeatLength = (uint8_t) readWordAndSkip(&cp);
  int totalSamples = codeLength * 2 + repeatLength * 2;
  if(totalSamples > sampleData->sampleCount) {
    return false;
  }
  sampleData->sampleCount = totalSamples;


  //  Read and convert actual samples
  for(int i = 0; i < totalSamples; i++) {
    sampleData->samples[i] = readWordAndSkip(&cp) * usec;
  }


  return true;
}
