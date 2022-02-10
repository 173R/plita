#include "VulkanSwapChain.hpp"
#include "VulkanDevice.hpp"

VulkanSwapChain::VulkanSwapChain(WindowSystem* window_system, VulkanDevice* device, const VkSurfaceKHR& surface):
  vk_swap_chain_(VK_NULL_HANDLE),
  device_(device),
  vk_surface_(surface),
  window_system_(window_system)
{

}

VulkanSwapChain::~VulkanSwapChain() {
  for (auto image_view : vk_image_views_) {
    vkDestroyImageView(device_->vk_device_, image_view, nullptr);
  }

  vkDestroySwapchainKHR(device_->vk_device_, vk_swap_chain_, nullptr);
}

/*static*/ VulkanSwapChain::SwapChainSupportDetails VulkanSwapChain::findSwapChainSupportDetails(
  const VkPhysicalDevice& device,
  const VkSurfaceKHR& surface
) {
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.vk_capabilities);

  uint32_t formats_count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formats_count, nullptr);

  if (formats_count != 0) {
    details.vk_formats.resize(formats_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formats_count, details.vk_formats.data());
  }

  uint32_t present_mode_count = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);

  if (present_mode_count != 0) {
    details.vk_present_modes.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.vk_present_modes.data());
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

void VulkanSwapChain::createSwapChain() {

  SwapChainSupportDetails details = findSwapChainSupportDetails(device_->vk_physical_device_, vk_surface_);

  QueueFamilyIndices indices = VulkanDevice::findQueueFamilies(device_->vk_physical_device_, vk_surface_);
  uint32_t queue_family_indices[] = {indices.graphics_family.value(), indices.present_family.value()};

  VkSurfaceFormatKHR surface_format = selectSurfaceFormat(details.vk_formats);
  vk_image_format_ = surface_format.format;
  VkPresentModeKHR present_mode = selectPresentMode(details.vk_present_modes);
  vk_image_extent_ = selectExtent(details.vk_capabilities, window_system_->window_);

  uint32_t image_count = details.vk_capabilities.minImageCount + 1;

  if (details.vk_capabilities.maxImageCount > 0 && image_count > details.vk_capabilities.maxImageCount) {
    image_count = details.vk_capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  create_info.surface = vk_surface_;
  create_info.minImageCount = image_count;
  create_info.imageFormat = vk_image_format_;
  create_info.imageColorSpace = surface_format.colorSpace;
  create_info.imageExtent = vk_image_extent_;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  if (indices.graphics_family != indices.present_family) {
    create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    create_info.queueFamilyIndexCount = 2;
    create_info.pQueueFamilyIndices = queue_family_indices;
  } else {
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
  }
  create_info.preTransform = details.vk_capabilities.currentTransform;
  create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  create_info.presentMode = present_mode;
  create_info.clipped = VK_TRUE;
  create_info.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(device_->vk_device_, &create_info, nullptr, &vk_swap_chain_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }

  vkGetSwapchainImagesKHR(device_->vk_device_, vk_swap_chain_, &image_count, nullptr);
  vk_images_.resize(image_count);
  vkGetSwapchainImagesKHR(device_->vk_device_, vk_swap_chain_, &image_count, vk_images_.data());
}

void VulkanSwapChain::createImageViews() {
  vk_image_views_.resize(vk_images_.size());

  for (size_t i = 0; i < vk_images_.size(); i++) {
    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.image = vk_images_[i];
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format = vk_image_format_;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;
    if (vkCreateImageView(device_->vk_device_, &create_info, nullptr, &vk_image_views_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create image views!");
    }

  }
}

