#pragma once

#include <tuple>

struct TxTarget {
    char family;
    uint32_t group;
    uint32_t device;

    bool operator==(const TxTarget &other) const {
      return std::tie(family, group, device) == std::tie(other.family, other.group, other.device);
    }
};

struct TxData : public TxTarget {
    bool state;

    TxData() = default;

    TxData(TxTarget &target, bool state) : TxTarget{target.family, target.group, target.device},
                                           state{state} {}
};

void tx_init();

void tx_submit(const TxData &);

void tx_process();

bool tx_get_state(const TxTarget &);