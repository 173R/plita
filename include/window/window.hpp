#pragma once
#include "GLFW/glfw3.h"

#include <cstdint>
#include <string>


class Window {
 private:
  const uint16_t height_;
  const uint16_t width_;
  const std::string title_;
  GLFWwindow* window_;

 public:
  Window(const uint16_t height, const uint16_t width, const std::string title);
  ~Window();
  
  bool isWindowClosed();
};