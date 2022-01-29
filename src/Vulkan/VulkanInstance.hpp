#include "vulkan/vulkan.h"
#include "glfw/glfw3.h"

#include <vector>

class VulkanInstance {
public:
  VulkanInstance();
  ~VulkanInstance();
private:
  VkInstance instance_;
};