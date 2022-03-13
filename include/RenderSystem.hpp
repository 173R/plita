#pragma once

#include "vulkan/vulkan.h"

#include "Singleton.hpp"
#include "VulkanDevice.hpp"
#include "VulkanWindow.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanCommandPool.hpp"

class RenderSystem: public Singleton<RenderSystem>  {
public:
  RenderSystem();
  ~RenderSystem();
  
  void init(WindowSystem* window_);

  /*TODO: Вынести*/
  void draw();
  size_t current_frame = 0;

private:
  VkInstance vk_instance_;

  VulkanDevice* device_;
  VulkanWindow* window_;
  VulkanSwapChain* swap_chain_;
  WindowSystem* window_system_;
  VulkanGraphicsPipeline* graphics_pipeline_;
  VulkanCommandPool* command_pool_;

  void initVkInstance();
  void initVkDevice();
  void initVkWindow();
  void initVkSwapChain();
  void initVkGraphicsPipeline();
  void initVkCommandPool();
  void destroySwapChain();
  void recreateSwapChain();
};