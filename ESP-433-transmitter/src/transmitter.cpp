#include <RCSwitch.h>
#include <Array.h>

#include "transmitter.hpp"

#define RADIO_TX_PIN        D8
#define TX_QUEUE_SIZE       20
#define STATE_CACHE_SIZE    20

static Array<TxData, TX_QUEUE_SIZE> txQueue;

static RCSwitch mySwitch = RCSwitch();
static Array<TxData, STATE_CACHE_SIZE> stateCache;

bool find_in_cache(const TxTarget &target, size_t &found_index) {
  for (size_t i = 0; i < stateCache.size(); ++i) {
    if (static_cast<TxTarget>(stateCache[i]) == target) {
      found_index = i;
      return true;
    }
  }
  return false;
}

void tx_init() {
  mySwitch.enableTransmit(RADIO_TX_PIN);
}

void tx_submit(const TxData &data) {
  noInterrupts();
  txQueue.push_back(data);

  size_t idx;
  if (find_in_cache(data, idx)) {
    stateCache[idx] = data;
  } else {
    stateCache.push_back(data);
  }
  interrupts();
}

void tx_process() {
  TxData tmpData;
  bool txNeeded = false;

  noInterrupts();
  if (!txQueue.empty()) {
    tmpData = txQueue.front();
    txQueue.remove(0);
    txNeeded = true;
  }
  interrupts();

  if (txNeeded) {
    Serial.printf("Transmit: %c %d %d, state: %d\n", tmpData.family, tmpData.group, tmpData.device, tmpData.state);
    if (tmpData.state) {
      mySwitch.switchOn(tmpData.family, tmpData.group, tmpData.device);
    } else {
      mySwitch.switchOff(tmpData.family, tmpData.group, tmpData.device);
    }
  }
}

bool tx_get_state(const TxTarget &target) {
  bool ret_state = false;  // Default to false
  size_t idx;

  noInterrupts();
  if (find_in_cache(target, idx)) {
    ret_state = stateCache[idx].state;
  }
  interrupts();

  return ret_state;
}