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
  VkInstance vk_instance_;
  VkDevice vk_device_;
  VkPhysicalDevice vk_physical_device_;
  VkSurfaceKHR vk_surface_;
  VkQueue vk_graphics_queue_;
  VkQueue vk_present_queue_;
  QueueFamilyIndices queue_indices_;

  explicit VulkanDevice(VkInstance& instance, const VkSurfaceKHR& surface);
  ~VulkanDevice();

  void createDevice();
  void setDeviceExtensions(std::vector<const char *>& extensions);

  static VkInstance createVkInstance();
  /** @find graphics and present queue family indices*/
  static QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

  /*TODO: переделать*/
  static const std::vector<const char*> validationLayers;
  static bool checkValidationLayerSupport();

private:
  /*Список требуемых расширений*/
  std::vector<const char*> device_extensions_;

  void selectPhysicalDevice();
  /*Проверка нужных параметров у GPU*/
  bool isSuitableDevice(const VkPhysicalDevice& device);
  /*Проверка, поддреживает ли GPU конкретные расширения*/
  bool checkDeviceExtensionSupport(const VkPhysicalDevice& device);

};
