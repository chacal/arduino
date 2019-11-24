#include <RCSwitch.h>
#include <Array.h>

#include "transmitter.hpp"

#define RADIO_TX_PIN        D8
#define STATE_CACHE_SIZE    20

static TxData txBuf;
volatile bool txNeeded = false;

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
  txBuf = data;
  txNeeded = true;
  size_t idx;
  if (find_in_cache(data, idx)) {
    stateCache[idx] = data;
  } else {
    stateCache.push_back(data);
  }
  interrupts();
}

void tx_process() {
  TxData tmpBuf;
  bool tmpNeeded;

  noInterrupts();
  tmpNeeded = txNeeded;
  tmpBuf = txBuf;
  txNeeded = false;
  interrupts();

  if (tmpNeeded) {
    Serial.printf("Transmit: %c %d %d, state: %d\n", tmpBuf.family, tmpBuf.group, tmpBuf.device, tmpBuf.state);
    if (tmpBuf.state) {
      mySwitch.switchOn(tmpBuf.family, tmpBuf.group, tmpBuf.device);
    } else {
      mySwitch.switchOff(tmpBuf.family, tmpBuf.group, tmpBuf.device);
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