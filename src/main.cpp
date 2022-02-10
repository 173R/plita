#include "PlitaRoot.hpp"

#include <iostream>

int main() {
  try {
    PlitaRoot plita;
    PlitaRoot::Instance()->initialize();

    while (!WindowSystem::Instance()->shouldClose()) {
      WindowSystem::Instance()->pollEvents();
      RenderSystem::Instance()->draw();
    }

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
