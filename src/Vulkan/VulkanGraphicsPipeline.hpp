#pragma once

#include <vulkan/vulkan.h>

#include "VulkanDevice.hpp"

#include <vector>
#include <fstream>
#include <string>

class VulkanGraphicsPipeline {
public:
  VulkanDevice* device_;
  VulkanSwapChain* swap_chain_;

  VkPipeline vk_graphics_pipeline_;
  VkRenderPass vk_render_pass_;
  VkPipelineLayout vk_pipeline_layout_;
  std::vector<VkFramebuffer> vk_framebuffers_;
  VkCommandPool vk_command_pool_;
  std::vector<VkCommandBuffer> vk_command_buffers_;
  VkSemaphore vk_image_available_semaphore_;
  VkSemaphore vk_render_finished_semaphore_;

  VulkanGraphicsPipeline(VulkanDevice* device, VulkanSwapChain* swap_chain);
  ~VulkanGraphicsPipeline();

  /*TODO:Пернести в файловую систему*/
  static std::vector<char> readFile(const std::string& filename);
  void createPipeline();

private:
  /*Чекнуть*/
  void createRenderPass();
  VkShaderModule createShaderModule(const std::vector<char>& code) const;


  void createFramebuffers();
  void createCommandPool();
  void createCommandBuffers();
  void createSemaphores();
};

