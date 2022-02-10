#pragma once

#include "GLFW/glfw3.h"

#include "Singleton.hpp"

#include <cstdint>
#include <string>

class WindowSystem: public Singleton<WindowSystem> {
public:
  const uint16_t height_;
  const uint16_t width_;
  const std::string title_;
  GLFWwindow* window_;

  WindowSystem();
  ~WindowSystem();

  void init(const std::string& title, uint16_t width, uint16_t height);
  void pollEvents();
  bool shouldClose();

private:
};
