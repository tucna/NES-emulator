#include "Mapper_000.h"

Mapper_000::Mapper_000(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks)
{
}

bool Mapper_000::CpuMapRead(uint16_t addr, uint32_t& mapped_addr)
{
  if (addr >= 0x8000 && addr <= 0xFFFF)
  {
    mapped_addr = addr & (m_PRGBanks > 1 ? 0x7FFF : 0x3FFF);
    return true;
  }

  return false;
}

bool Mapper_000::CpuMapWrite(uint16_t addr, uint32_t& mapped_addr)
{
  if (addr >= 0x8000 && addr <= 0xFFFF)
  {
    mapped_addr = addr & (m_PRGBanks > 1 ? 0x7FFF : 0x3FFF);
    return true;
  }

  return false;
}

bool Mapper_000::PpuMapRead(uint16_t addr, uint32_t& mapped_addr)
{
  if (addr >= 0x0000 && addr <= 0x1FFF)
  {
    mapped_addr = addr;
    return true;
  }

  return false;
}

bool Mapper_000::PpuMapWrite(uint16_t addr, uint32_t& mapped_addr)
{
  if (addr >= 0x0000 && addr <= 0x1FFF)
  {
    if (m_CHRBanks == 0)
    {
      // Treat as RAM
      mapped_addr = addr;
      return true;
    }
  }

  return false;
}
