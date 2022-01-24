#include "engine/engine.hpp"

const std::string WINDOW_TITLE = "Plita the GAME";

Engine::Engine() {
  ///////////////////////////
  // vulkan initialization //
  ///////////////////////////

  // !!!TODO: find out window width and height!!!
  const uint16_t width = 1280;
  const uint16_t height = 720;
  window_ = new Window(width, height, WINDOW_TITLE);
  event_processor_ = new EventProcessor();
}

void Engine::run() {
  while (!window_->shouldClose()) {
    // drawing (buffer swap?)
    event_processor_->processEvents();
  }
}

Engine::~Engine() {
  delete event_processor_;
  delete window_;
  ////////////////////////
  // ¿ vulkan cleanup ? //
  ////////////////////////
  glfwTerminate();
}
