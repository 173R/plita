#include "VulkanDevice.hpp"

VulkanDevice::VulkanDevice(VkInstance& instance) :
  instance_(instance), 
  physical_device_(VK_NULL_HANDLE),
  device_(VK_NULL_HANDLE)
{
    
}

VulkanDevice::~VulkanDevice() {
  vkDestroyDevice(device_, nullptr);
}

VkInstance VulkanDevice::createInstance() {
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Plita";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Plita engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  /*TODO: Передавать расширения в параметрах*/
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtensions;
  createInfo.enabledLayerCount = 0;

  VkInstance instance;
  /*TODO: Обработать ошибку создания*/
  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    throw std::runtime_error("VK_INSTANCE_CREATE: Failed to create");
  }

  return instance;
}

void VulkanDevice::selectPhysicalDevice() {
  uint32_t deviceCount = 0;

  vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

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
  QueueFamilyIndices indices = findQueueFamilies(device);

  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);

  return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
    && indices.isComplete();
}

void VulkanDevice::createDevice() {
  selectPhysicalDevice();

  queue_indices_ = findQueueFamilies(physical_device_);
  float queuePriority = 1.0f;

  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = queue_indices_.graphicsFamily.value();
  queueCreateInfo.queueCount = 1;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.queueCreateInfoCount = 1;
  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount = 0;
  createInfo.enabledLayerCount = 0;

  if (vkCreateDevice(physical_device_, &createInfo, nullptr, &device_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }

  vkGetDeviceQueue(device_, queue_indices_.graphicsFamily.value(), 0, &graphics_queue_);
}

/*Поиск графических и present очередей*/
QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

  for (uint32_t i = 0; i < queueFamilies.size(); i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
      indices.presentFamily = i;
      break;
    }
  }

  return indices;
}