#pragma once

#include <cstdint>

class Mapper
{
public:
  Mapper(uint8_t prgBanks, uint8_t chrBanks) : m_PRGBanks(prgBanks), m_CHRBanks(chrBanks) {}
  ~Mapper() {}

  // Transform CPU bus address into PRG ROM offset
  virtual void MapReadByCPU(uint16_t addr, uint32_t& mapped_addr) = 0;
  virtual void MapWriteByCPU(uint16_t addr, uint32_t& mapped_addr) = 0;

  // Transform PPU bus address into CHR ROM offset
  virtual void MapReadByPPU(uint16_t addr, uint32_t& mapped_addr) = 0;
  virtual void MapWriteByPPU(uint16_t addr, uint32_t& mapped_addr) = 0;

protected:
  uint8_t m_PRGBanks;
  uint8_t m_CHRBanks;
};

