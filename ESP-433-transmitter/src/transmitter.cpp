#include <RCSwitch.h>

#include "transmitter.hpp"

#define RADIO_TX_PIN   D0

static TxData txBuf;
volatile bool txNeeded = false;

static RCSwitch mySwitch = RCSwitch();

void tx_init() {
  mySwitch.enableTransmit(RADIO_TX_PIN);
}

void tx_submit(const TxData &data) {
  noInterrupts();
  txBuf = data;
  txNeeded = true;
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

