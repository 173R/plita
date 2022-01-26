#pragma once

#include "PlitaWindow.hpp"

class PlitaRoot {
private:
  const PlitaWindow* window_;

public:
  PlitaRoot();

  void run();

  ~PlitaRoot();
};
