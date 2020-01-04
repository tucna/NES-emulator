#pragma once

#include "engine/tPixelGameEngine.h"

class NES;

class Visualizer : public tDX::PixelGameEngine
{
public:
  Visualizer(NES* nes);
  ~Visualizer() {}

  bool OnUserCreate() override;
  bool OnUserUpdate(float fElapsedTime) override;
  bool OnUserUpdateEndFrame(float fElapsedTime) override;
  bool OnUserDestroy() override;

private:
  NES* m_nes;
};
