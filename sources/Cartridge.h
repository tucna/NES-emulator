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
  enum class Mirroring
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

  Mirroring GetMirroring() { return m_mirroring; }

private:
  union Flags6
  {
    struct
    {
      uint8_t mirroring               : 1;
      uint8_t battery                 : 1;
      uint8_t trainer                 : 1;
      uint8_t ignore_mirroring        : 1;
      uint8_t lower_nibble_mapper_ID  : 4;
    };

    uint8_t flags;
  };

  union Flags7
  {
    struct
    {
      uint8_t vs_unisystem : 1;
      uint8_t reserved : 3;
      uint8_t higher_nibble_mapper_ID : 4;
    };

    uint8_t flags;
  };

  struct Header
  {
    // String "NES^Z" used to recognize .NES files
    char name[4];
    // Number of 16kB ROM banks
    uint8_t prgRomBanks;
    // Number of 8kB VROM banks
    uint8_t chrRomBanks;
    /*
      bit 0     1 for vertical mirroring, 0 for horizontal mirroring.
      bit 1     1 for battery-backed RAM at $6000-$7FFF.
      bit 2     1 for a 512-byte trainer at $7000-$71FF.
      bit 3     1 for a four-screen VRAM layout.
      bit 4-7   Four lower bits of ROM Mapper Type.
    */
    Flags6 flags6;
    /*
      bit 0     1 for VS-System cartridges.
      bit 1-3   Reserved, must be zeroes!
      bit 4-7   Four higher bits of ROM Mapper Type.
    */
    Flags7 flags7;
    /*
      Number of 8kB RAM banks. For compatibility with the previous
      versions of the .NES format, assume 1x8kB RAM page when this
      byte is zero.
    */
    uint8_t prgRamBanks;
    /*
      bit 0     1 for PAL cartridges, otherwise assume NTSC.
      bit 1-7   Reserved, must be zeroes!
    */
    uint8_t flags9;
    // TV system, PRG-RAM presence (unofficial, rarely used extension)
    uint8_t flags10;
    // Unused padding
    char unused[5];
  };

  static constexpr uint8_t Flag6_Mirroring  = 0b00000001;
  static constexpr uint8_t Flag6_Trainer    = 0b00000100;

  Header m_header;

  bool m_imageValid = false;
  bool m_debugCartridge = false;

  uint8_t m_mapperID = 0;

  Mirroring m_mirroring = Mirroring::Horizontal;

  std::vector<uint8_t> m_PRG_ROM;
  std::vector<uint8_t> m_CHR_ROM;
  std::vector<uint8_t> m_CHR_RAM;

  std::unique_ptr<Mapper> m_mapper;

  Bus* m_bus = nullptr;
};

