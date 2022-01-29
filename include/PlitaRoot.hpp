#pragma once

#include "PlitaWindow.hpp"

class PlitaRoot {
public:
  static void initialize();
  static PlitaWindow& getWindow();

private:
  PlitaRoot();
  ~PlitaRoot();

  void initWindow(const uint16_t width, const uint16_t height, const std::string title);
  void destroyWindow();

  static PlitaRoot instance_;
  
  PlitaWindow* window_;
};