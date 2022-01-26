#include "PlitaWindow.hpp"

#include <stdexcept>

PlitaWindow::PlitaWindow(const uint16_t width, const uint16_t height, const std::string title)
  : width_(width), height_(height), title_(title) {

  if (glfwInit() == GLFW_FALSE) {
    throw std::runtime_error("WIN_INIT: Failed to initialize the GLFW library.");
  }
  if (glfwVulkanSupported == GLFW_FALSE) {
    throw std::runtime_error("WIN_INIT: GLFW failed to find Vulkan support.");
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (window_ == nullptr) {
    throw std::runtime_error("WIN_INIT: Failed to create window");
  }
}

bool PlitaWindow::shouldClose() const {
  return glfwWindowShouldClose(window_);
}

PlitaWindow::~PlitaWindow() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}
