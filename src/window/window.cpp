#include "window/window.hpp"

#include <stdexcept>

const std::string GLFW_INITIALIZATION_ERROR_MESSAGE = "WINDOW::CONSTRUCTOR: Failed to initialize the GLFW library.";
const std::string      VULKAN_SUPPORT_ERROR_MESSAGE = "WINDOW::CONSTRUCTOR: GLFW failed to find Vulkan support.";
const std::string     WINDOW_CREATION_ERROR_MESSAGE = "WINDOW::CONSTRUCTOR: Failed to create window with title: ";

Window::Window(const uint16_t height, const uint16_t width, const std::string title)
  : height_(height), width_(width), title_(title) {
  if (glfwInit() == GLFW_FALSE) {
    throw std::runtime_error(GLFW_INITIALIZATION_ERROR_MESSAGE);
  }
  if (glfwVulkanSupported == GLFW_FALSE) {
    throw std::runtime_error(VULKAN_SUPPORT_ERROR_MESSAGE);
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (window_ == nullptr) {
    throw std::runtime_error(WINDOW_CREATION_ERROR_MESSAGE + title);
  }
}

Window::~Window() {
  glfwDestroyWindow(window_);
  // !!!TODO: move to application destructor!!!
  glfwTerminate();
}

bool Window::shouldClose() {
  return glfwWindowShouldClose(window_) == GLFW_TRUE;
}
