#pragma once

#include "GLFW/glfw3.h"

#include <cstdint>
#include <string>

// Class for window initialization and state.
class Window {
private:
  // Window height.
  const uint16_t height_;
  // Window width.
  const uint16_t width_;
  // Window window title.
  const std::string title_;
  // GLFW window pointer.
  GLFWwindow* window_;

public:
  // @brief Initializes the GLFW window if possible.
  // 
  // @param height - Height of window in pixels.
  // @param  width - Width of window in pixels.
  // @param  title - Window title.
  Window(const uint16_t width, const uint16_t height, const std::string title);

  // @brief Destroys the GLFW window.
  ~Window();

  // @brief Checks if GLFW window should close.
  // 
  // @return Window should close flag.
  bool shouldClose() const;
};
