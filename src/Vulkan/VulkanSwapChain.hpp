#pragma once

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

#include "WindowSystem.hpp"

#include <vector>
#include <algorithm>

class VulkanDevice;

class VulkanSwapChain {
public:
  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR vk_capabilities;
    std::vector<VkSurfaceFormatKHR> vk_formats;
    std::vector<VkPresentModeKHR> vk_present_modes;
  };

  std::vector<VkImage> vk_images_;
  std::vector<VkImageView> vk_image_views_;
  VkSwapchainKHR vk_swap_chain_;
  VkFormat vk_image_format_{};
  VkExtent2D vk_image_extent_{};

  VulkanSwapChain(WindowSystem* window_system, VulkanDevice* device, const VkSurfaceKHR& surface);
  ~VulkanSwapChain();

  static SwapChainSupportDetails findSwapChainSupportDetails(const VkPhysicalDevice& device, const VkSurfaceKHR& surface);

  void createSwapChain();
  void createImageViews();
private:
  const VkSurfaceKHR& vk_surface_;
  VulkanDevice* device_;
  WindowSystem* window_system_;

  /**@select surface format for swap chain*/
  VkSurfaceFormatKHR selectSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
  /**@select surface present mode for swap chain*/
  VkPresentModeKHR selectPresentMode(const std::vector<VkPresentModeKHR>& mods);
  /**@select extent for swap chain*/
  VkExtent2D selectExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow *window);
};

