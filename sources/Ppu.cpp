#include "Bus.h"
#include "Cartridge.h"
#include "Ppu.h"

Ppu::Ppu() :
  m_frameComplete(false),
  m_cycle(0),
  m_scanline(0),
  m_sprScreen(256, 240),
  m_sprPatternTable{{128, 128}, {128, 128}}
{
  m_palScreen[0x00] = tDX::Pixel(84, 84, 84);
  m_palScreen[0x01] = tDX::Pixel(0, 30, 116);
  m_palScreen[0x02] = tDX::Pixel(8, 16, 144);
  m_palScreen[0x03] = tDX::Pixel(48, 0, 136);
  m_palScreen[0x04] = tDX::Pixel(68, 0, 100);
  m_palScreen[0x05] = tDX::Pixel(92, 0, 48);
  m_palScreen[0x06] = tDX::Pixel(84, 4, 0);
  m_palScreen[0x07] = tDX::Pixel(60, 24, 0);
  m_palScreen[0x08] = tDX::Pixel(32, 42, 0);
  m_palScreen[0x09] = tDX::Pixel(8, 58, 0);
  m_palScreen[0x0A] = tDX::Pixel(0, 64, 0);
  m_palScreen[0x0B] = tDX::Pixel(0, 60, 0);
  m_palScreen[0x0C] = tDX::Pixel(0, 50, 60);
  m_palScreen[0x0D] = tDX::Pixel(0, 0, 0);
  m_palScreen[0x0E] = tDX::Pixel(0, 0, 0);
  m_palScreen[0x0F] = tDX::Pixel(0, 0, 0);

  m_palScreen[0x10] = tDX::Pixel(152, 150, 152);
  m_palScreen[0x11] = tDX::Pixel(8, 76, 196);
  m_palScreen[0x12] = tDX::Pixel(48, 50, 236);
  m_palScreen[0x13] = tDX::Pixel(92, 30, 228);
  m_palScreen[0x14] = tDX::Pixel(136, 20, 176);
  m_palScreen[0x15] = tDX::Pixel(160, 20, 100);
  m_palScreen[0x16] = tDX::Pixel(152, 34, 32);
  m_palScreen[0x17] = tDX::Pixel(120, 60, 0);
  m_palScreen[0x18] = tDX::Pixel(84, 90, 0);
  m_palScreen[0x19] = tDX::Pixel(40, 114, 0);
  m_palScreen[0x1A] = tDX::Pixel(8, 124, 0);
  m_palScreen[0x1B] = tDX::Pixel(0, 118, 40);
  m_palScreen[0x1C] = tDX::Pixel(0, 102, 120);
  m_palScreen[0x1D] = tDX::Pixel(0, 0, 0);
  m_palScreen[0x1E] = tDX::Pixel(0, 0, 0);
  m_palScreen[0x1F] = tDX::Pixel(0, 0, 0);

  m_palScreen[0x20] = tDX::Pixel(236, 238, 236);
  m_palScreen[0x21] = tDX::Pixel(76, 154, 236);
  m_palScreen[0x22] = tDX::Pixel(120, 124, 236);
  m_palScreen[0x23] = tDX::Pixel(176, 98, 236);
  m_palScreen[0x24] = tDX::Pixel(228, 84, 236);
  m_palScreen[0x25] = tDX::Pixel(236, 88, 180);
  m_palScreen[0x26] = tDX::Pixel(236, 106, 100);
  m_palScreen[0x27] = tDX::Pixel(212, 136, 32);
  m_palScreen[0x28] = tDX::Pixel(160, 170, 0);
  m_palScreen[0x29] = tDX::Pixel(116, 196, 0);
  m_palScreen[0x2A] = tDX::Pixel(76, 208, 32);
  m_palScreen[0x2B] = tDX::Pixel(56, 204, 108);
  m_palScreen[0x2C] = tDX::Pixel(56, 180, 204);
  m_palScreen[0x2D] = tDX::Pixel(60, 60, 60);
  m_palScreen[0x2E] = tDX::Pixel(0, 0, 0);
  m_palScreen[0x2F] = tDX::Pixel(0, 0, 0);

  m_palScreen[0x30] = tDX::Pixel(236, 238, 236);
  m_palScreen[0x31] = tDX::Pixel(168, 204, 236);
  m_palScreen[0x32] = tDX::Pixel(188, 188, 236);
  m_palScreen[0x33] = tDX::Pixel(212, 178, 236);
  m_palScreen[0x34] = tDX::Pixel(236, 174, 236);
  m_palScreen[0x35] = tDX::Pixel(236, 174, 212);
  m_palScreen[0x36] = tDX::Pixel(236, 180, 176);
  m_palScreen[0x37] = tDX::Pixel(228, 196, 144);
  m_palScreen[0x38] = tDX::Pixel(204, 210, 120);
  m_palScreen[0x39] = tDX::Pixel(180, 222, 120);
  m_palScreen[0x3A] = tDX::Pixel(168, 226, 144);
  m_palScreen[0x3B] = tDX::Pixel(152, 226, 180);
  m_palScreen[0x3C] = tDX::Pixel(160, 214, 228);
  m_palScreen[0x3D] = tDX::Pixel(160, 162, 160);
  m_palScreen[0x3E] = tDX::Pixel(0, 0, 0);
  m_palScreen[0x3F] = tDX::Pixel(0, 0, 0);
}

void Ppu::Clock()
{
  // Fake some noise for now
  m_sprScreen.SetPixel(m_cycle - 1, m_scanline, m_palScreen[(rand() % 2) ? 0x3F : 0x30]);

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

void Ppu::ConnectCartridge(Cartridge * cartridge)
{
  m_cartridge = cartridge;
}

tDX::Sprite& Ppu::GetPatternTable(uint8_t i, uint8_t palette)
{
  // TODO
  return m_sprPatternTable[i];
}

uint8_t Ppu::ReadByCPU(uint16_t addr, bool rdonly)
{
  return uint8_t();
}

void Ppu::WriteByCPU(uint16_t addr, uint8_t data)
{
}

uint8_t Ppu::ReadByPPU(uint16_t addr, bool rdonly)
{
  uint8_t data = 0x00;
  addr &= 0x3FFF;

  m_cartridge->ReadByPPU(addr, data);

  return data;
}

void Ppu::WriteByPPU(uint16_t addr, uint8_t data)
{
  addr &= 0x3FFF;

  m_cartridge->WriteByPPU(addr, data);
}
