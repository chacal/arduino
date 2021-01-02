#include "moving-average.hpp"

#include <Arduino.h>

int      *window;
uint16_t windowSize = 0;
uint16_t readIndex  = 0;              // the index of the current reading

namespace movingAvg {
  void init(uint16_t size) {
    window     = (int *) malloc(size * sizeof(int));
    windowSize = size;

    for (uint16_t i = 0; i < windowSize; ++i) {
      window[i] = 0;
    }
  }

  float calculate(int newValue) {
    window[readIndex] = newValue;
    readIndex++;
    if (readIndex >= windowSize) {
      readIndex = 0;
    }

    uint32_t total         = 0;
    uint16_t readingsCount = 0;

    for (uint16_t i = 0; i < windowSize; ++i) {
      if (window[i] > 0) {
        total += window[i];
        readingsCount++;
      }
    }

    return total / (float) readingsCount;
  }
}
