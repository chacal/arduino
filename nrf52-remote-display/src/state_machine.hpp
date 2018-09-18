#pragma once

#include "states/idle.hpp"
#include "states/start.hpp"

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
  states::Context ctx;
  states::M::PeerRoot<states::start, states::idle> hfsm_root;
};


namespace states {
  template<typename T>
  void Context::react(const T &event) {
    fsm->react(event);
  }
}