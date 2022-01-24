#include "window/window.hpp"

#include <stdexcept>

Window::Window(const uint16_t height, const uint16_t width, const std::string title)
	: height_(height), width_(width), title_(title) {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (window_ == nullptr) {
		throw std::runtime_error("WINDOW::CONSTRUCTOR: Failed to create window " + title);
	}
}
Window::~Window() {
	glfwDestroyWindow(window_);
	glfwTerminate();
}

bool Window::isWindowClosed() {
	return glfwWindowShouldClose(window_);
}

