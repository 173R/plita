#pragma once

// Class for keyboard and controller events processing.
class EventProcessor {
private:
public:
  // @brief Creates event processor.
  EventProcessor();

  // @brief Processes keyboard and controller events.
  void processEvents() const;

  ~EventProcessor();
};
