#define T_PGE_APPLICATION
#include "../engine/tPixelGameEngine.h"

class NESEmulator : public tDX::PixelGameEngine
{
public:
  NESEmulator()
  {
    sAppName = "NES_Emulator";
  }

  bool OnUserCreate() override
  {
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override
  {
    return true;
  }

private:
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  NESEmulator nes;
  if (nes.Construct(400, 300, 2, 2))
    nes.Start();

  return 0;
}