#include "VulkanDevice.hpp"

#include "iostream"

/*TODO: перенести*/
const std::vector<const char*> VulkanDevice::validationLayers = {
  "VK_LAYER_KHRONOS_validation"
};

VulkanDevice::VulkanDevice(VkInstance& instance, const VkSurfaceKHR& surface) :
  vk_instance_(instance),
  vk_physical_device_(VK_NULL_HANDLE),
  vk_device_(VK_NULL_HANDLE),
  vk_surface_(surface),
  vk_graphics_queue_(VK_NULL_HANDLE),
  vk_present_queue_(VK_NULL_HANDLE)
{
    
}

VulkanDevice::~VulkanDevice() {
  vkDestroyDevice(vk_device_, nullptr);
}

VkInstance VulkanDevice::createVkInstance() {

  if (!checkValidationLayerSupport()) {
    throw std::runtime_error("validation layers requested, but not available!");
  }

  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Plita";
  app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.pEngineName = "Plita engine";
  app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  app_info.apiVersion = VK_API_VERSION_1_0;

  /*TODO: Передавать расширения в параметрах*/
  uint32_t glfw_extension_count = 0;
  const char** glfw_extensions;
  glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

  VkInstanceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  create_info.pApplicationInfo = &app_info;
  create_info.enabledExtensionCount = glfw_extension_count;
  create_info.ppEnabledExtensionNames = glfw_extensions;
  create_info.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
  create_info.ppEnabledLayerNames = validationLayers.data();

  VkInstance instance;
  /*TODO: Обработать ошибку создания*/
  if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
    throw std::runtime_error("VK_INSTANCE_CREATE: Failed to create");
  }

  return instance;
}

void VulkanDevice::selectPhysicalDevice() {
  uint32_t device_count = 0;

  vkEnumeratePhysicalDevices(vk_instance_, &device_count, nullptr);

  if (device_count == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(vk_instance_, &device_count, devices.data());

  for (const VkPhysicalDevice& device : devices) {
    if (isSuitableDevice(device)) {
      vk_physical_device_ = device;
      VkPhysicalDeviceProperties device_properties;
      vkGetPhysicalDeviceProperties(device, &device_properties);
      std::cout << device_properties.deviceName << std::endl;
      break;
    }
  }

  if (vk_physical_device_ == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}

bool VulkanDevice::isSuitableDevice(const VkPhysicalDevice& device) {
  QueueFamilyIndices indices = findQueueFamilies(device, vk_surface_);
  VulkanSwapChain::SwapChainSupportDetails details = VulkanSwapChain::findSwapChainSupportDetails(device, vk_surface_);

  VkPhysicalDeviceProperties device_properties;
  vkGetPhysicalDeviceProperties(device, &device_properties);

  return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
    && indices.isComplete()
    && checkDeviceExtensionSupport(device)
    && !details.vk_formats.empty()
    && !details.vk_present_modes.empty();
}

bool VulkanDevice::checkDeviceExtensionSupport(const VkPhysicalDevice& device) {
  uint32_t extension_count = 0;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
  std::vector<VkExtensionProperties> available_extensions(extension_count);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

  std::set<std::string> required_extensions(device_extensions_.begin(), device_extensions_.end());

  for (const auto& extension : available_extensions) {
    required_extensions.erase(extension.extensionName);
  }

  return required_extensions.empty();
}

void VulkanDevice::setDeviceExtensions(std::vector<const char *>& extensions) {
  device_extensions_ = extensions;
}

void VulkanDevice::createDevice() {
  selectPhysicalDevice();

  queue_indices_ = findQueueFamilies(vk_physical_device_, vk_surface_);
  float queue_priority = 1.0f;

  VkDeviceQueueCreateInfo queue_create_info{};
  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.queueFamilyIndex = queue_indices_.graphics_family.value();
  queue_create_info.queueCount = 1;
  queue_create_info.pQueuePriorities = &queue_priority;

  VkPhysicalDeviceFeatures device_features{};

  VkDeviceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pQueueCreateInfos = &queue_create_info;
  create_info.queueCreateInfoCount = 1;
  create_info.pEnabledFeatures = &device_features;
  create_info.enabledLayerCount = 0;
  create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions_.size());
  create_info.ppEnabledExtensionNames = device_extensions_.data();

  if (vkCreateDevice(vk_physical_device_, &create_info, nullptr, &vk_device_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }

  vkGetDeviceQueue(vk_device_, queue_indices_.graphics_family.value(), 0, &vk_graphics_queue_);
  vkGetDeviceQueue(vk_device_, queue_indices_.present_family.value(), 0, &vk_present_queue_);
}

/*static*/ QueueFamilyIndices VulkanDevice::findQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) {
  QueueFamilyIndices indices;
  VkBool32 present_support = false;
  uint32_t queue_family_count = 0;

  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

  for (uint32_t i = 0; i < queue_families.size(); i++) {
    if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      if (!indices.graphics_family.has_value()) {
        indices.graphics_family = i;
      }
    }
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
    if (present_support) {
      indices.present_family = i;
    }
    if (indices.isComplete()) {
      break;
    }
  }

  if (!indices.isComplete()) {
    throw std::runtime_error("Present queue unsupported");
  }

  return indices;
}

bool VulkanDevice::checkValidationLayerSupport() {

  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const char* layerName : validationLayers) {
    bool layerFound = false;

    for (const auto& layerProperties : availableLayers) {
      if (strcmp(layerName, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }

  return true;
}