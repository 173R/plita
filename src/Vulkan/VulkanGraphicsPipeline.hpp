#pragma once

#include <vulkan/vulkan.h>

#include "VulkanDevice.hpp"

#include <vector>
#include <fstream>
#include <string>

class VulkanGraphicsPipeline {
public:
  VkPipeline vk_graphics_pipeline_;
  VkRenderPass vk_render_pass_;
  VkPipelineLayout vk_pipeline_layout_;

  VulkanGraphicsPipeline(VulkanDevice* device, VulkanSwapChain* swap_chain);
  ~VulkanGraphicsPipeline();

  /*TODO:Пернести в файловую систему*/
  static std::vector<char> readFile(const std::string& filename);
  void createPipeline();

private:
  VulkanDevice* device_;
  VulkanSwapChain* swap_chain_;

  /*Чекнуть*/
  void createRenderPass();
  VkShaderModule createShaderModule(const std::vector<char>& code) const;
};

