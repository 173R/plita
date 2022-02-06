#pragma once

#include "vulkan/vulkan.h"

#include "VulkanDevice.hpp"
#include "VulkanWindow.hpp"
#include "VulkanSwapChain.hpp"

class RenderSystem {
public:   
  RenderSystem();
  ~RenderSystem();
  
  void init(WindowSystem* window_);

private:
  static bool initialised_;

  VkInstance vk_instance_;
  VulkanDevice* vk_device_;
  VulkanWindow* vk_window_;
  VulkanSwapChain* vk_swap_chain_;
  WindowSystem* window_system_;

  void initVkInstance();
  void initVkDevice();
  void initVkWindow();
  void initVkSwapChain();
};