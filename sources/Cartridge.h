/*
  Addressing modes, lookup table and official instructions are taken from NES emulator of javidx9 - OneLoneCoder.com.
  Thank you for the great work!
*/

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Mapper.h"

class Bus;

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

  // Communication with Main Bus
  void ReadByCPU(uint16_t addr, uint8_t &data);
  void WriteByCPU(uint16_t addr, uint8_t data);

  // Communication with PPU Bus
  void ReadByPPU(uint16_t addr, uint8_t &data);
  void WriteByPPU(uint16_t addr, uint8_t data);

  void ConnectToBus(Bus* bus) { m_bus = bus; }

  Mirror GetMirroring() { return m_mirror; }

private:
  bool m_imageValid = false;
  bool m_debugCartridge = false;

  uint8_t m_mapperID = 0;
  uint8_t m_PRGBanks = 0;
  uint8_t m_CHRBanks = 0;

  Mirror m_mirror = Mirror::Horizontal;

  std::vector<uint8_t> m_PRGMemory;
  std::vector<uint8_t> m_CHRMemory;

  std::unique_ptr<Mapper> m_mapper;

  Bus* m_bus = nullptr;
};

