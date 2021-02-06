#pragma once

#include "states/start.hpp"
#include "states/discover.hpp"
#include "states/configure.hpp"
#include "states/connected.hpp"

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

  using conf = configure<M>;
  using disc = discover<M, conf>;
  using sta = start<M, disc>;
  M::PeerRoot <sta, disc, conf, connected> hfsm_root;
};
