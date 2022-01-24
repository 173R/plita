#include <iostream>
#include "glm/glm.hpp"
#include "glfw/glfw3.h"
#include "vulkan/vulkan.h"

int main() {
  std::cout << "Hello" << std::endl;

  try {
    //app.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}