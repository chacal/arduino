#pragma once

class PowerManager {
private:
  PowerManager();

public:
  static PowerManager &getInstance();
  void manage();
  void shutdown();
};
