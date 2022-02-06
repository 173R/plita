#pragma once

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include "VulkanSwapChain.hpp"

#include <vector> 
#include <stdexcept>
#include <optional>
#include <string>
#include <set>

struct QueueFamilyIndices {
  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;

  bool isComplete() const {
    return graphics_family.has_value() && present_family.has_value();
  }
};


class VulkanDevice {
public:
  VkInstance instance_;
  VkDevice device_;
  VkPhysicalDevice physical_device_;
  VkSurfaceKHR surface_;
  VkQueue graphics_queue_; //handle
  VkQueue present_queue_; //handle
  QueueFamilyIndices queue_indices_;

  explicit VulkanDevice(VkInstance& instance);
  ~VulkanDevice();

  static VkInstance createInstance();

  void createDevice();
  void setDeviceExtensions(std::vector<const char *>& extensions);
  void setSurface(const VkSurfaceKHR& surface);

  /** @find graphics and present queue family indices*/
  static QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

private:
  /*Список требуемых расширений*/
  std::vector<const char*> device_extensions_;

  void selectPhysicalDevice();
  /*Проверка нужных параметров у GPU*/
  bool isSuitableDevice(const VkPhysicalDevice& device);
  /*Проверка, поддреживает ли GPU конкретные расширения*/
  bool checkDeviceExtensionSupport(const VkPhysicalDevice& device);

};
