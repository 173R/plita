#include "VulkanWindow.hpp"

VulkanWindow::VulkanWindow():
  surface_(VK_NULL_HANDLE),
  vk_device_(nullptr),
  window_system_(nullptr)
{

}

VulkanWindow::~VulkanWindow() {
  vkDestroySurfaceKHR(vk_device_->instance_, surface_, nullptr);
}

void VulkanWindow::createSurface() {
  if (glfwCreateWindowSurface(vk_device_->instance_, window_system_->window_, nullptr, &surface_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }

  VkBool32 presentSupport = false;
  vkGetPhysicalDeviceSurfaceSupportKHR(
    vk_device_->physical_device_,
    vk_device_->queue_indices_.presentFamily.value(),
    surface_,
    &presentSupport
  );

  if (!presentSupport) {
    throw std::runtime_error("Present queue unsupported");
  }

}

void VulkanWindow::setDevice(VulkanDevice* device) {
  vk_device_ = device;
}

void VulkanWindow::setWindow(WindowSystem* window_system) {
  window_system_ = window_system;
}
