#pragma once

#include "vulkan/vulkan.h"

#include "VulkanDevice.hpp"
#include "VulkanWindow.hpp"

class RenderSystem {
public:   
  RenderSystem();
  ~RenderSystem();
  
  void init(WindowSystem* window_);

private:
  static bool initialised_;
  WindowSystem* window_system_;

  VkInstance vk_instance_;
  VulkanDevice* vk_device_;
  VulkanWindow* vk_window_;

  void initVkInstance();
  void initVkDevice();
  void initVkWindow();
};