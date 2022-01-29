#include "Vulkan/VulkanInstance.hpp"

class RenderSystem {
public:   
  RenderSystem();
  ~RenderSystem();
private:
  VkInstance vk_instance_;
};