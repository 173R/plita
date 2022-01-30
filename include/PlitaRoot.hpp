#pragma once

#include "WindowSystem.hpp"
#include "RenderSystem.hpp"


class PlitaRoot {
public:
  static void initialize();
  static WindowSystem& getWindow();

private:
  PlitaRoot();
  ~PlitaRoot();

  void initWindow();
  void initRenderSystem();

  static PlitaRoot instance_;
  
  WindowSystem* window_system_;
  RenderSystem* render_system_;
};