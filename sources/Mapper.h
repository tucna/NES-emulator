#pragma once

#include <cstdint>

class Mapper
{
public:
  Mapper(uint8_t prgBanks, uint8_t chrBanks) : m_PRGBanks(prgBanks), m_CHRBanks(chrBanks) {}
  ~Mapper() {}

  // Transform CPU bus address into PRG ROM offset
  virtual bool CpuMapRead(uint16_t addr, uint32_t& mapped_addr) = 0;
  virtual bool CpuMapWrite(uint16_t addr, uint32_t& mapped_addr) = 0;

  // Transform PPU bus address into CHR ROM offset
  virtual bool PpuMapRead(uint16_t addr, uint32_t& mapped_addr) = 0;
  virtual bool PpuMapWrite(uint16_t addr, uint32_t& mapped_addr) = 0;

protected:
  uint8_t m_PRGBanks;
  uint8_t m_CHRBanks;
};

