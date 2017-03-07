#pragma once

#define MAX_RAW_SAMPLES 200

struct RawSampleData {
  unsigned int samples[MAX_RAW_SAMPLES];
  uint8_t sampleCount = MAX_RAW_SAMPLES;
  uint8_t freq;
};

bool convertProntoToRaw(const char *prontoHex, RawSampleData *sampleData);
