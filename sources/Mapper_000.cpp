#include "Mapper_000.h"

Mapper_000::Mapper_000(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks)
{
}

void Mapper_000::MapReadByCPU(uint16_t addr, uint32_t& mapped_addr)
{
  mapped_addr = addr & (m_PRGBanks > 1 ? 0x7FFF : 0x3FFF); // TODO write and read are the same
}

void Mapper_000::MapWriteByCPU(uint16_t addr, uint32_t& mapped_addr)
{
  mapped_addr = addr & (m_PRGBanks > 1 ? 0x7FFF : 0x3FFF); // TODO write and read are the same
}

void Mapper_000::MapReadByPPU(uint16_t addr, uint32_t& mapped_addr)
{
  mapped_addr = addr;
}

void Mapper_000::MapWriteByPPU(uint16_t addr, uint32_t& mapped_addr)
{
  if (m_CHRBanks == 0)
  {
    // Treat as RAM
    mapped_addr = addr;
  }
}
