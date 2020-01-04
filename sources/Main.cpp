#define T_PGE_APPLICATION
#include "engine/tPixelGameEngine.h"

#include "NES.h"
#include "Visualizer.h"

using namespace std;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  unique_ptr<NES> nes = make_unique<NES>();
  unique_ptr<Visualizer> visualizer = make_unique<Visualizer>(nes.get());

  return 0;
}