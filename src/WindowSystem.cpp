#include "WindowSystem.hpp"

#include <stdexcept>

WindowSystem::WindowSystem(): 
  width_(0), height_(0), title_("Window") {
}

WindowSystem::~WindowSystem() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

void WindowSystem::init(const std::string title, const uint16_t width, const uint16_t height) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (window_ == nullptr) {
    throw std::runtime_error("WIN_INIT: Failed to create window");
  }
}

bool WindowSystem::shouldClose() {
  return glfwWindowShouldClose(window_);
}

void WindowSystem::pollEvents() {
  glfwPollEvents();
}

