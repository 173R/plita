#include "VulkanWindow.hpp"

VulkanWindow::VulkanWindow(const VkInstance& vk_instance, WindowSystem* window_system):
  vk_surface_(VK_NULL_HANDLE),
  vk_instance_(vk_instance),
  window_system_(window_system)
{

}

VulkanWindow::~VulkanWindow() {
  vkDestroySurfaceKHR(vk_instance_, vk_surface_, nullptr);
}

void VulkanWindow::createSurface() {
  if (glfwCreateWindowSurface(vk_instance_, window_system_->window_, nullptr, &vk_surface_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}
