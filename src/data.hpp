#pragma once
#include <array>

struct Data{
  double d[7];
  void set(const std::array<double,6>&);
  void setAcc(const std::array<double,3>&);
};
