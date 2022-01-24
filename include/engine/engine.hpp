#pragma once

#include "engine/event_processor.hpp"
#include "window/window.hpp"

// Game engine class.
class Engine {
private:
  // Event processor pointer.
  const EventProcessor* event_processor_;
  // Window pointer.
  const Window* window_;

public:
  // @brief Initializes game.
  Engine();

  // @brief Main application loop.
  void run();

  // @brief Terminates engine and whole application.
  ~Engine();
};
