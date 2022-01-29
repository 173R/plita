#include "PlitaWindow.hpp"

#include <stdexcept>

PlitaWindow::PlitaWindow(const uint16_t width, const uint16_t height, const std::string title)
  : width_(width), height_(height), title_(title) {

  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (window_ == nullptr) {
    throw std::runtime_error("WIN_INIT: Failed to create window");
  }
}

PlitaWindow::~PlitaWindow() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

bool PlitaWindow::shouldClose() {
  return glfwWindowShouldClose(window_);
}

void PlitaWindow::pollEvents() {
  glfwPollEvents();
}

