#pragma once

#include "vulkan/vulkan.h"
#include "glfw/glfw3.h"

#include "VulkanDevice.hpp"
#include "WindowSystem.hpp"

class VulkanWindow {
public:
  VulkanWindow();
  ~VulkanWindow();

  void createSurface();
  void setDevice(VulkanDevice* vk_device);
  void setWindow(WindowSystem* window_system);

private:
  VulkanDevice* vk_device_;
  WindowSystem* window_system_;
  VkSurfaceKHR surface_;
};