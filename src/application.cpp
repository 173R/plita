#include "engine/engine.hpp"

#include <iostream>

// @brief Application entrypoint.
// 
// @return Exit code.
int main() {
  Engine* engine = new Engine();

  try {
    engine->run();
    delete engine;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
