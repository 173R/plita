#include "engine/event_processor.hpp"

#include "GLFW/glfw3.h"

EventProcessor::EventProcessor() {

}

void EventProcessor::processEvents() const {
  glfwPollEvents();
}

EventProcessor::~EventProcessor() {

}
