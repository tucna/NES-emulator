#include "Bus.h"

Bus::Bus()
{
}

void Bus::Write(uint16_t addr, uint8_t data)
{
  if (addr >= 0x0000 && addr <= 0xFFFF)
    m_ram[addr] = data;
}

uint8_t Bus::Read(uint16_t addr, bool bReadOnly)
{
  if (addr >= 0x0000 && addr <= 0xFFFF)
    return m_ram[addr];

  return 0x00;
}

void Bus::WriteToRAM(size_t index, uint8_t data)
{
  m_ram[index] = data;
}
