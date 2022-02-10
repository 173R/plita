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
  //vkDeviceWaitIdle(vk_device_->device_);

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

void RenderSystem::draw() {
  uint32_t image_index;
  vkAcquireNextImageKHR(
    device_->vk_device_,
    swap_chain_->vk_swap_chain_,
    UINT64_MAX,
    graphics_pipeline_->vk_image_available_semaphore_,
    VK_NULL_HANDLE,
    &image_index
  );

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore wait_semaphores[] = {graphics_pipeline_->vk_image_available_semaphore_};
  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  submit_info.pWaitDstStageMask = wait_stages;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &graphics_pipeline_->vk_command_buffers_[image_index];

  VkSemaphore signal_semaphores[] = {graphics_pipeline_->vk_render_finished_semaphore_};
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_semaphores;

  if (vkQueueSubmit(device_->vk_graphics_queue_, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
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
}

