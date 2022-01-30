#pragma once

#include "vulkan/vulkan.h"
#include "glfw/glfw3.h"

#include <vector> 
#include <stdexcept>
#include <optional>

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

class VulkanDevice {
public:
  VkInstance instance_;
  VkDevice device_;
  VkPhysicalDevice physical_device_;
  VkQueue graphics_queue_;
  QueueFamilyIndices queue_indices_;

  VulkanDevice(VkInstance& instance);
  ~VulkanDevice();

  static VkInstance createInstance();
  void createDevice();

private:

  void selectPhysicalDevice();
  /*Проверка нужных параметров у GPU*/
  bool isSuitableDevice(const VkPhysicalDevice& device);
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};
