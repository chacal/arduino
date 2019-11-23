#pragma once

struct TxData {
    char family;
    uint32_t group;
    uint32_t device;
    bool state;
};

void tx_init();

void tx_submit(const TxData &);

void tx_process();

