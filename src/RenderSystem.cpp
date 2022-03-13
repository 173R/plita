#include "RenderSystem.hpp"

#include <iostream>

template<>
RenderSystem* Singleton<RenderSystem>::instance_ = nullptr;

RenderSystem::RenderSystem():
  vk_instance_(VK_NULL_HANDLE),
  device_(nullptr),
  window_(nullptr),
  window_system_(nullptr),
  swap_chain_(nullptr),
  graphics_pipeline_(nullptr)
{

}

RenderSystem::~RenderSystem() {
  vkDeviceWaitIdle(device_->vk_device_);

  delete command_pool_;
  command_pool_ = nullptr;

  delete graphics_pipeline_;
  graphics_pipeline_ = nullptr;

  delete swap_chain_;
  swap_chain_ = nullptr;

  delete device_;
  device_ = nullptr;

  delete window_;
  window_ = nullptr;

  vkDestroyInstance(vk_instance_, nullptr);
  std::cout << "RenderSystem deleted" << std::endl;
}

void RenderSystem::init(WindowSystem* window_system) {

  window_system_ = window_system;
  initVkInstance();
  initVkWindow();
  initVkDevice();
  initVkSwapChain();
  initVkGraphicsPipeline();
  initVkCommandPool();
}

void RenderSystem::initVkInstance() {
  vk_instance_ = VulkanDevice::createVkInstance();
}

void RenderSystem::initVkDevice() {
  device_ = new VulkanDevice(vk_instance_, window_->vk_surface_);
  std::vector<const char *> device_extensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  };
  device_->setDeviceExtensions(device_extensions);
  device_->createDevice();
}

void RenderSystem::initVkWindow() {
  window_ = new VulkanWindow(vk_instance_, window_system_);
  window_->createSurface();
}

void RenderSystem::initVkSwapChain() {
  swap_chain_ = new VulkanSwapChain(window_system_, device_, window_->vk_surface_);
  swap_chain_->createSwapChain();
  swap_chain_->createImageViews();
}

void RenderSystem::initVkGraphicsPipeline() {
  graphics_pipeline_ = new VulkanGraphicsPipeline(device_, swap_chain_);
  graphics_pipeline_->createPipeline();
}

void RenderSystem::initVkCommandPool() {
  command_pool_ = new VulkanCommandPool(device_, swap_chain_, graphics_pipeline_);
  command_pool_->createFramebuffers();
  command_pool_->createCommandPool();
  command_pool_->createCommandBuffers();
  command_pool_->createSemaphores();
}

void RenderSystem::draw() {
  vkWaitForFences(device_->vk_device_, 1, &command_pool_->in_flight_fences_[current_frame], VK_TRUE, UINT64_MAX);

  uint32_t image_index;
  VkResult result = vkAcquireNextImageKHR(
    device_->vk_device_,
    swap_chain_->vk_swap_chain_,
    UINT64_MAX,
    command_pool_->vk_image_available_semaphores_[current_frame],
    VK_NULL_HANDLE,
    &image_index
  );

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  if (command_pool_->images_in_flight[image_index] != VK_NULL_HANDLE) {
    vkWaitForFences(device_->vk_device_, 1, &command_pool_->images_in_flight[image_index], VK_TRUE, UINT64_MAX);
  }
  command_pool_->images_in_flight[image_index] = command_pool_->in_flight_fences_[current_frame];

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore wait_semaphores[] = {command_pool_->vk_image_available_semaphores_[current_frame]};
  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  submit_info.pWaitDstStageMask = wait_stages;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_pool_->vk_command_buffers_[image_index];

  VkSemaphore signal_semaphores[] = {command_pool_->vk_render_finished_semaphores_[current_frame]};
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_semaphores;

  vkResetFences(device_->vk_device_, 1, &command_pool_->in_flight_fences_[current_frame]);

  result = vkQueueSubmit(device_->vk_graphics_queue_, 1, &submit_info, command_pool_->in_flight_fences_[current_frame]);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    recreateSwapChain();
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signal_semaphores;

  VkSwapchainKHR swapChains[] = {swap_chain_->vk_swap_chain_};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &image_index;

  vkQueuePresentKHR(device_->vk_present_queue_, &presentInfo);

  current_frame = (current_frame + 1) % command_pool_->MAX_FRAMES_IN_FLIGHT;
  //vkQueueWaitIdle(device_->vk_present_queue_);
}

void RenderSystem::recreateSwapChain() {
  destroySwapChain();

  initVkSwapChain();
  initVkGraphicsPipeline();
  command_pool_->setSwapChain(swap_chain_);
  command_pool_->setGraphicsPipeline(graphics_pipeline_);
  command_pool_->createFramebuffers();
  command_pool_->createCommandBuffers();
}

void RenderSystem::destroySwapChain() {
  vkDeviceWaitIdle(device_->vk_device_);

  vkFreeCommandBuffers(
    device_->vk_device_,
    command_pool_->vk_command_pool_,
    static_cast<uint32_t>(command_pool_->vk_command_buffers_.size()),
    command_pool_->vk_command_buffers_.data()
  );

  for (auto framebuffer : command_pool_->vk_framebuffers_) {
    vkDestroyFramebuffer(device_->vk_device_, framebuffer, nullptr);
  }

  delete graphics_pipeline_;
  graphics_pipeline_ = nullptr;

  delete swap_chain_;
  swap_chain_ = nullptr;
}

