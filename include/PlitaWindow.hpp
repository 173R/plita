#pragma once

#include "GLFW/glfw3.h"

#include <cstdint>
#include <string>

class PlitaWindow {
private:
  const uint16_t height_;
  const uint16_t width_;
  const std::string title_;
  GLFWwindow* window_;

public:
  PlitaWindow(const uint16_t width, const uint16_t height, const std::string title);
  ~PlitaWindow();

  void pollEvents();
  bool shouldClose();
};
