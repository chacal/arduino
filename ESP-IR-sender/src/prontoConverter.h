#ifndef ESP_IR_SENDER_PRONTOCONVERTER_H
#define ESP_IR_SENDER_PRONTOCONVERTER_H

#define MAX_RAW_SAMPLES 200

struct RawSampleData {
    uint16_t samples[MAX_RAW_SAMPLES];
    uint8_t sampleCount = MAX_RAW_SAMPLES;
    uint8_t freq;
};

bool convertProntoToRaw(const char *prontoHex, RawSampleData *sampleData);

#endif //ESP_IR_SENDER_PRONTOCONVERTER_H
