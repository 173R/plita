#include "RenderSystem.hpp"

#include <iostream>
#include <cassert>

bool RenderSystem::initialised_ = false;

RenderSystem::RenderSystem() {
  /*TODO: другое решение*/
  assert(!initialised_);
  initialised_ = true;
  std::cout << "RenderSystem init" << std::endl;
}

RenderSystem::~RenderSystem() {
  delete vk_window_;
  delete vk_device_;
  vkDestroyInstance(vk_instance_, nullptr);
  std::cout << "RenderSystem deleted" << std::endl;
}

void RenderSystem::init(WindowSystem* window_system) {
  window_system_ = window_system;
  initVkInstance();
  initVkDevice();
  initVkWindow();
}

void RenderSystem::initVkInstance() {
  vk_instance_ = VulkanDevice::createInstance();
}

void RenderSystem::initVkDevice() {
  vk_device_ = new VulkanDevice(vk_instance_);
  vk_device_->createDevice();
}

void RenderSystem::initVkWindow() {
  vk_window_ = new VulkanWindow();
  vk_window_->setDevice(vk_device_);
  vk_window_->setWindow(window_system_);
  vk_window_->createSurface();
}