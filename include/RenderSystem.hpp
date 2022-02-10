#pragma once

#include "vulkan/vulkan.h"

#include "Singleton.hpp"
#include "VulkanDevice.hpp"
#include "VulkanWindow.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanGraphicsPipeline.hpp"

class RenderSystem: public Singleton<RenderSystem>  {
public:
  RenderSystem();
  ~RenderSystem();
  
  void init(WindowSystem* window_);

  void draw();

private:
  VkInstance vk_instance_;

  VulkanDevice* device_;
  VulkanWindow* window_;
  VulkanSwapChain* swap_chain_;
  WindowSystem* window_system_;
  VulkanGraphicsPipeline* graphics_pipeline_;

  void initVkInstance();
  void initVkDevice();
  void initVkWindow();
  void initVkSwapChain();
  void initVkGraphicsPipeline();
};