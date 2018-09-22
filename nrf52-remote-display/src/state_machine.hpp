#pragma once

#include "states/base.hpp"
#include "states/idle.hpp"
#include "states/start.hpp"
#include "states/adv_with_whitelist.hpp"
#include "states/discoverable.hpp"
#include "states/connected.hpp"
#include "display.hpp"

using namespace fsm;
using namespace states;

class state_machine {
public:
  state_machine(const display &d) : ctx{this, d}, hfsm_root{ctx} {}

  bool update() {
    return hfsm_root.update();
  }

  template<typename T>
  void react(const T &event) {
    hfsm_root.react(event);
  }

  void react(const ble_evt_t *p_ble_evt) {
    NRF_LOG_INFO("BLE event: %d", p_ble_evt->header.evt_id);
    hfsm_root.react(p_ble_evt);
  }

private:
  Context ctx;
  M::PeerRoot <
  M::Composite<base,
      start,
      idle,
      adv_with_whitelist,
      discoverable,
      connected
  >
  >       hfsm_root;
};


namespace fsm {
  template<typename T>
  void Context::react(const T &event) {
    fsm->react(event);
  }
}