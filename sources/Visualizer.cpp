#include "../engine/tPixelGameEngine.h"

#include "NES.h"
#include "Visualizer.h"

Visualizer::Visualizer(NES* nes) :
  m_nes(nes)
{
  sAppName = "NES_Emulator";

  if (Construct(400, 300, 2, 2))
    Start();
}

bool Visualizer::OnUserCreate()
{
  return true;
}

bool Visualizer::OnUserUpdate(float fElapsedTime)
{
  return true;
}
