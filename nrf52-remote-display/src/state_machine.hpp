#pragma once

#include "states/idle.hpp"
#include "states/start.hpp"

using namespace fsm;
using namespace states;

class state_machine {
public:
  state_machine() : ctx{this}, hfsm_root{ctx} {}

  bool update() {
    return hfsm_root.update();
  }

  template<typename T>
  void react(const T &event) {
    hfsm_root.react(event);
  }

private:
  Context ctx;
  M::PeerRoot <start, idle> hfsm_root;
};


namespace fsm {
  template<typename T>
  void Context::react(const T &event) {
    fsm->react(event);
  }
}