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
  // @brief Initializes the GLFW window if possible.
  // 
  // @param height - Height of window in pixels.
  // @param  width - Width of window in pixels.
  // @param  title - Window title.
  Window(const uint16_t height, const uint16_t width, const std::string title);

  // @brief Destroys the GLFW window.
  ~Window();

  // @brief Checks if GLFW window is closed.
  // 
  // @return Window should close flag.
  bool shouldClose();
};
