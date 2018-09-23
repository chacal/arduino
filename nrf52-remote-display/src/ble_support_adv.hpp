#pragma once

namespace ble_support::adv {
  void init();

  void connectable_start();

  void discoverable_start();

  void stop();
}