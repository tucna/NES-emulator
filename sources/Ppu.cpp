#include "Ppu.h"

Ppu::Ppu() :
  m_frameComplete(false),
  m_cycle(0),
  m_scanline(0),
  m_bus(nullptr)
{
}

void Ppu::Clock()
{
  // Fake some noise for now
  //sprScreen.SetPixel(cycle - 1, scanline, palScreen[(rand() % 2) ? 0x3F : 0x30]);

  // Advance renderer - it never stops, it's relentless
  m_cycle++;

  if (m_cycle >= 341)
  {
    m_cycle = 0;
    m_scanline++;

    if (m_scanline >= 261)
    {
      m_scanline = -1;
      m_frameComplete = true;
    }
  }
}
