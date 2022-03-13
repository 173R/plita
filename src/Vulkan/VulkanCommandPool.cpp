#include "VulkanCommandPool.hpp"

VulkanCommandPool::VulkanCommandPool(VulkanDevice* device, VulkanSwapChain* swap_chain, VulkanGraphicsPipeline* graphics_pipeline):
  swap_chain_(swap_chain),
  device_(device),
  graphics_pipeline_(graphics_pipeline),
  vk_command_pool_(VK_NULL_HANDLE)
{

}

VulkanCommandPool::~VulkanCommandPool() {
  //vkDeviceWaitIdle(device_->vk_device_);
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(device_->vk_device_, vk_render_finished_semaphores_[i], nullptr);
    vkDestroySemaphore(device_->vk_device_, vk_image_available_semaphores_[i], nullptr);
    vkDestroyFence(device_->vk_device_, in_flight_fences_[i], nullptr);
  }
  vkDestroyCommandPool(device_->vk_device_, vk_command_pool_, nullptr);
  for (auto framebuffer : vk_framebuffers_) {
    vkDestroyFramebuffer(device_->vk_device_, framebuffer, nullptr);
  }
}

void VulkanCommandPool::createCommandPool() {
  VkCommandPoolCreateInfo pool_info{};
  pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_info.queueFamilyIndex = device_->queue_indices_.graphics_family.value();

  if (vkCreateCommandPool(device_->vk_device_, &pool_info, nullptr, &vk_command_pool_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

void VulkanCommandPool::createFramebuffers() {
  vk_framebuffers_.resize(swap_chain_->vk_images_.size());

  for (size_t i = 0; i < swap_chain_->vk_image_views_.size(); i++) {
    VkImageView attachments[] = {
      swap_chain_->vk_image_views_[i]
    };

    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = graphics_pipeline_->vk_render_pass_;
    framebuffer_info.attachmentCount = 1;
    framebuffer_info.pAttachments = attachments;
    framebuffer_info.width = swap_chain_->vk_image_extent_.width;
    framebuffer_info.height = swap_chain_->vk_image_extent_.height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(device_->vk_device_, &framebuffer_info, nullptr, &vk_framebuffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

void VulkanCommandPool::createCommandBuffers() {
  vk_command_buffers_.resize(vk_framebuffers_.size());

  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = vk_command_pool_;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = static_cast<uint32_t>(vk_command_buffers_.size());

  if (vkAllocateCommandBuffers(device_->vk_device_, &alloc_info, vk_command_buffers_.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  for (size_t i = 0; i < vk_command_buffers_.size(); i++) {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(vk_command_buffers_[i], &begin_info) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = graphics_pipeline_->vk_render_pass_;
    render_pass_info.framebuffer = vk_framebuffers_[i];
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = swap_chain_->vk_image_extent_;

    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clearColor;

    vkCmdBeginRenderPass(vk_command_buffers_[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(vk_command_buffers_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_->vk_graphics_pipeline_);

    vkCmdDraw(vk_command_buffers_[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(vk_command_buffers_[i]);

    if (vkEndCommandBuffer(vk_command_buffers_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }
}

void VulkanCommandPool::createSemaphores() {
  vk_image_available_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
  vk_render_finished_semaphores_.resize(MAX_FRAMES_IN_FLIGHT);
  in_flight_fences_.resize(MAX_FRAMES_IN_FLIGHT);
  images_in_flight.resize(swap_chain_->vk_images_.size(), VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphore_info{};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence_info{};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(device_->vk_device_, &semaphore_info, nullptr, &vk_image_available_semaphores_[i]) != VK_SUCCESS
      || vkCreateSemaphore(device_->vk_device_, &semaphore_info, nullptr, &vk_render_finished_semaphores_[i]) != VK_SUCCESS
      || vkCreateFence(device_->vk_device_, &fence_info, nullptr, &in_flight_fences_[i]) != VK_SUCCESS
    ) {

      throw std::runtime_error("failed to create semaphores!");
    }
  }
}

void VulkanCommandPool::setSwapChain(VulkanSwapChain *swapChain) {
  swap_chain_ = swapChain;

}

void VulkanCommandPool::setGraphicsPipeline(VulkanGraphicsPipeline *graphics_pipeline) {
  graphics_pipeline_ = graphics_pipeline;
}
