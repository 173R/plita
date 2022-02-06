#pragma once

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include "VulkanDevice.hpp"
#include "WindowSystem.hpp"

class VulkanWindow {
public:
  VulkanWindow();
  ~VulkanWindow();

  void createSurface();
  void setInstance(const VkInstance& vk_instance);
  void setWindow(WindowSystem* window_system);

  VkSurfaceKHR surface_;
private:
  VkInstance vk_instance_;
  WindowSystem* window_system_;
};