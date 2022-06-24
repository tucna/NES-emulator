#include "Emulator.h"

using namespace std;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  Emulator emulator;

  if (emulator.Construct(500, 480, 1, 1))
    emulator.Start();

  return 0;
}