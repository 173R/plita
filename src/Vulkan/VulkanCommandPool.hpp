#pragma once

#include "vulkan/vulkan.h"

#include "VulkanSwapChain.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanDevice.hpp"

#include <vector>

class VulkanCommandPool {
public:
  VulkanCommandPool(VulkanDevice* device, VulkanSwapChain* swap_chain, VulkanGraphicsPipeline* graphics_pipeline);
  ~VulkanCommandPool();

  const int MAX_FRAMES_IN_FLIGHT = 2;

  VkCommandPool vk_command_pool_;
  std::vector<VkFramebuffer> vk_framebuffers_;
  std::vector<VkCommandBuffer> vk_command_buffers_;
  std::vector<VkSemaphore> vk_image_available_semaphores_;
  std::vector<VkSemaphore> vk_render_finished_semaphores_;
  std::vector<VkFence> in_flight_fences_;
  std::vector<VkFence> images_in_flight;

  void createFramebuffers();
  void createCommandPool();
  void createCommandBuffers();
  void setSwapChain(VulkanSwapChain *swap_chain);
  void setGraphicsPipeline(VulkanGraphicsPipeline *graphics_pipeline);
  /*Перенести создание семафоров*/
  void createSemaphores();
private:
  VulkanSwapChain* swap_chain_;
  VulkanGraphicsPipeline* graphics_pipeline_;
  VulkanDevice* device_;
};