#pragma once

#include "WindowSystem.hpp"
#include "RenderSystem.hpp"


class PlitaRoot: public Singleton<PlitaRoot> {
public:
  PlitaRoot();
  ~PlitaRoot();

  void initialize();
private:
  WindowSystem* window_system_;
  RenderSystem* render_system_;
};