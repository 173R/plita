#include "VulkanDevice.hpp"

VulkanDevice::VulkanDevice(VkInstance& instance) :
  instance_(instance),
  physical_device_(VK_NULL_HANDLE),
  device_(VK_NULL_HANDLE),
  surface_(VK_NULL_HANDLE),
  graphics_queue_(VK_NULL_HANDLE),
  present_queue_(VK_NULL_HANDLE)
{
    
}

VulkanDevice::~VulkanDevice() {
  vkDestroyDevice(device_, nullptr);
}

VkInstance VulkanDevice::createInstance() {
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
  create_info.enabledLayerCount = 0;

  VkInstance instance;
  /*TODO: Обработать ошибку создания*/
  if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
    throw std::runtime_error("VK_INSTANCE_CREATE: Failed to create");
  }

  return instance;
}

void VulkanDevice::selectPhysicalDevice() {
  uint32_t device_count = 0;

  vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);

  if (device_count == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> devices(device_count);
  vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

  for (const VkPhysicalDevice& device : devices) {
    if (isSuitableDevice(device)) {
      physical_device_ = device;
      break;
    }
  }

  if (physical_device_ == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}

bool VulkanDevice::isSuitableDevice(const VkPhysicalDevice& device) {
  QueueFamilyIndices indices = findQueueFamilies(device, surface_);
  VulkanSwapChain::SwapChainSupportDetails details = VulkanSwapChain::findSwapChainSupportDetails(device, surface_);

  VkPhysicalDeviceProperties device_properties;
  vkGetPhysicalDeviceProperties(device, &device_properties);

  return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
    && indices.isComplete()
    && checkDeviceExtensionSupport(device)
    && !details.formats.empty()
    && !details.present_modes.empty();
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

  queue_indices_ = findQueueFamilies(physical_device_, surface_);
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

  if (vkCreateDevice(physical_device_, &create_info, nullptr, &device_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }

  vkGetDeviceQueue(device_, queue_indices_.graphics_family.value(), 0, &graphics_queue_);
  vkGetDeviceQueue(device_, queue_indices_.present_family.value(), 0, &present_queue_);
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

  if (!indices.present_family.has_value()) {
    throw std::runtime_error("Present queue unsupported");
  }

  return indices;
}

void VulkanDevice::setSurface(const VkSurfaceKHR& surface) {
  surface_ = surface;
}
