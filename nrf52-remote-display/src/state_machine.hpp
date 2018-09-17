#pragma once

#include "states/idle.hpp"
#include "states/start.hpp"

class state_machine {
public:
  state_machine() : machine(ctx) {}

  bool update() {
    return machine.update();
  }

private:
  states::Context ctx;
  states::M::PeerRoot<states::Start, states::idle> machine;
};
