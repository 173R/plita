#pragma once

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include "WindowSystem.hpp"

#include <vector>
#include <algorithm>

/*TODO: Желательно убрать*/
class VulkanDevice;

class VulkanSwapChain {
public:
  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
  };

  VulkanSwapChain();
  ~VulkanSwapChain();

  static SwapChainSupportDetails findSwapChainSupportDetails(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

  void createSwapChain(const WindowSystem* window_system, const VkSurfaceKHR& surface, const VkPhysicalDevice& physical_device_);

private:
  /**@select surface format for swap chain*/
  VkSurfaceFormatKHR selectSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
  /**@select surface present mode for swap chain*/
  VkPresentModeKHR selectPresentMode(const std::vector<VkPresentModeKHR>& mods);
  /**@select extent for swap chain*/
  VkExtent2D selectExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow *window);
};

