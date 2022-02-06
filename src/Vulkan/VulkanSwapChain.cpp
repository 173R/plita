#include "VulkanSwapChain.hpp"

VulkanSwapChain::VulkanSwapChain() {

}

VulkanSwapChain::~VulkanSwapChain() {

}

/*static*/ VulkanSwapChain::SwapChainSupportDetails VulkanSwapChain::findSwapChainSupportDetails(
  const VkPhysicalDevice& device,
  const VkSurfaceKHR& surface
) {
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

  uint32_t formats_count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formats_count, nullptr);

  if (formats_count != 0) {
    details.formats.resize(formats_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formats_count, details.formats.data());
  }

  uint32_t present_mode_count = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);

  if (present_mode_count != 0) {
    details.present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.present_modes.data());
  }

  return details;
}

VkSurfaceFormatKHR VulkanSwapChain::selectSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {

  for (const auto& format : formats) {
    if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return format;
    }
  }

  return formats[0];
}

VkPresentModeKHR VulkanSwapChain::selectPresentMode(const std::vector<VkPresentModeKHR>& mods) {
  for (const auto& mod : mods) {
    if (mod == VK_PRESENT_MODE_MAILBOX_KHR) {
      return mod;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChain::selectExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  } else {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D extent = {
      static_cast<uint32_t>(width),
      static_cast<uint32_t>(height)
    };

    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return extent;
  }
}

void VulkanSwapChain::createSwapChain(
  const WindowSystem* window_system,
  const VkSurfaceKHR& surface,
  const VkPhysicalDevice& physical_device
) {
  SwapChainSupportDetails details = findSwapChainSupportDetails(physical_device, surface);
  QueueFamilyIndices indices = VulkanDevice::findQueueFamilies(physical_device, surface);
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

  VkSurfaceFormatKHR surface_format = selectSurfaceFormat(details.formats);
  VkPresentModeKHR present_mode = selectPresentMode(details.present_modes);
  VkExtent2D extent = selectExtent(details.capabilities, window_system->window_);

  uint32_t image_count = details.capabilities.minImageCount + 1;

  if (details.capabilities.maxImageCount > 0 && image_count > details.capabilities.maxImageCount) {
    image_count = details.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = surface;
  create_info.minImageCount = image_count;
  create_info.imageFormat = surface_format.format;
  create_info.imageColorSpace = surface_format.colorSpace;
  create_info.imageExtent = extent;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
}

