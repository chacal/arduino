#pragma once

#include <utility>

template<typename T, typename parameter>

class named_type {
public:
  explicit named_type(T const &value) : val(value) {}

  explicit named_type(T &&value) : val(std::move(value)) {}

  T &get() { return val; }

  T const &get() const { return val; }

private:
  T val;
};