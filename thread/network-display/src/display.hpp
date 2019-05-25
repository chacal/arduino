#pragma once

#include "u8g2_rendering.hpp"


class display : public u8g2_rendering {
public:
  virtual void on() = 0;

  virtual void render() = 0;

  virtual void clear() = 0;
};
