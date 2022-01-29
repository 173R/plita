#include "PlitaRoot.hpp"

#include <iostream>

int main() {
  try {

    PlitaRoot::initialize();

    while (!PlitaRoot::getWindow().shouldClose()) {
      PlitaRoot::getWindow().pollEvents();
    }

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
