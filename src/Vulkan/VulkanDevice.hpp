#pragma once

#include "vulkan/vulkan.h"
#include "glfw/glfw3.h"

#include <vector> 
#include <stdexcept>
#include <optional>
#include <string>
#include <set>

struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;

  bool isComplete() {
    return graphics_family.has_value() && present_family.has_value();
  }
};

class VulkanDevice {
public:
  VkInstance instance_;
  VkDevice device_;
  VkPhysicalDevice physical_device_;
  VkQueue graphics_queue_;
  VkQueue present_queue_;
  QueueFamilyIndices queue_indices_;

  VulkanDevice(VkInstance& instance);
  ~VulkanDevice();

  static VkInstance createInstance();
  void createDevice();
  void setDeviceExtensions(std::vector<char *>& extensions);

private:
  std::vector<char*> device_extensions_;
  std::vector<char*> instance_extensions_;

  void selectPhysicalDevice();
  /*Проверка нужных параметров у GPU*/
  bool isSuitableDevice(const VkPhysicalDevice& device);
  /*Проверка, поддреживает ли GPU конкретные расширения*/
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
};
