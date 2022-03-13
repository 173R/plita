#include "WindowSystem.hpp"

#include <stdexcept>
#include <iostream>

template<>
WindowSystem* Singleton<WindowSystem>::instance_ = nullptr;

bool WindowSystem::window_resized_ = false;

WindowSystem::WindowSystem(): 
  width_(0),
  height_(0), 
  title_("Window"),
  window_(nullptr)
{
}

WindowSystem::~WindowSystem() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

/*static*/ void WindowSystem::windowSizeCallback(GLFWwindow* window, int width, int height) {
  /*TODO: Не работает, переделать, обработать сворачивание окна*/
  std::cout << "width: " << width << std::endl << "height: " << height << std::endl;
  //window_resized_ = true;
}

void WindowSystem::init(const std::string& title, uint16_t width, uint16_t height) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (window_ == nullptr) {
    throw std::runtime_error("WIN_INIT: Failed to create window");
  }

  glfwSetWindowSizeCallback(window_, windowSizeCallback);
}

bool WindowSystem::shouldClose() const {
  return glfwWindowShouldClose(window_);
}

void WindowSystem::pollEvents() const {
  glfwPollEvents();
}

