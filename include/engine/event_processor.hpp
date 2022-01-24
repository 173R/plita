#pragma once

// Class for keyboard and controller events processing.
class EventProcessor {
private:
  ////////////////////////////
  // controller type maybe? //
  ////////////////////////////

  /////////////////////////////////////////
  // some glfw_button to real_button map //
  /////////////////////////////////////////
public:
  // @brief Creates event processor.
  EventProcessor();

  // @brief Processes keyboard and controller events.
  void processEvents() const;

  ~EventProcessor();
};
