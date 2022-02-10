#pragma once

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include "VulkanDevice.hpp"
#include "WindowSystem.hpp"

class VulkanWindow {
public:
  VulkanWindow(const VkInstance& vk_instance, WindowSystem* window_system);
  ~VulkanWindow();

  void createSurface();

  VkSurfaceKHR vk_surface_;
private:
  VkInstance vk_instance_;
  WindowSystem* window_system_;
};