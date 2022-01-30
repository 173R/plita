#include "PlitaRoot.hpp"

PlitaRoot PlitaRoot::instance_;

PlitaRoot::PlitaRoot() {}
PlitaRoot::~PlitaRoot() {
  delete instance_.window_system_;
  delete instance_.render_system_;
}

void PlitaRoot::initialize() {
  instance_.initWindow();
  instance_.initRenderSystem();
}

WindowSystem& PlitaRoot::getWindow() {
  return *instance_.window_system_;
}

void PlitaRoot::initWindow() {
  instance_.window_system_ = new WindowSystem();
  instance_.window_system_->init("Game", 800, 600);
}

void PlitaRoot::initRenderSystem() {
  instance_.render_system_ = new RenderSystem();
  instance_.render_system_->init(instance_.window_system_);
}
