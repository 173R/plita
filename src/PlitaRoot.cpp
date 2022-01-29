#include "PlitaRoot.hpp"
#include "PlitaWindow.hpp"

PlitaRoot PlitaRoot::instance_;

PlitaRoot::PlitaRoot() {}
PlitaRoot::~PlitaRoot() {
  instance_.destroyWindow();
}

void PlitaRoot::initialize() {
  instance_.initWindow(800, 600, "GAME");
}

PlitaWindow& PlitaRoot::getWindow() {
  return *instance_.window_;
}

void PlitaRoot::initWindow(const uint16_t width, const uint16_t height, const std::string title) {
  instance_.window_ = new PlitaWindow(width, height, title);
}

void PlitaRoot::destroyWindow() {
  delete instance_.window_;
};


