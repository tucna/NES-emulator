#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Mapper.h"

class Cartridge
{
public:
  enum class Mirror
  {
    Horizontal,
    Vertical,
    OneScreen_Lo,
    OneScreen_Hi,
  };

  Cartridge(const std::string& file);
  ~Cartridge() {}

  bool IsImageValid() const { return m_imageValid; }

private:
  bool m_imageValid;

  uint8_t m_mapperID;
  uint8_t m_PRGBanks = 0;
  uint8_t m_CHRBanks = 0;

  Mirror m_mirror;

  std::vector<uint8_t> m_PRGMemory;
  std::vector<uint8_t> m_CHRMemory;

  std::unique_ptr<Mapper> m_mapper;
};

