#include "PlitaRoot.hpp"

template<>
PlitaRoot* Singleton<PlitaRoot>::instance_ = nullptr;

PlitaRoot::PlitaRoot():
  window_system_(new WindowSystem()),
  render_system_(new RenderSystem())
{}

PlitaRoot::~PlitaRoot() {
  delete window_system_;
  window_system_ = nullptr;

  delete render_system_;
  render_system_ = nullptr;
}

void PlitaRoot::initialize() {
  WindowSystem::Instance()->init("Game", 800, 600);
  RenderSystem::Instance()->init(WindowSystem::Instance());
}
