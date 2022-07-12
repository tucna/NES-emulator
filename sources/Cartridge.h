#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Mapper.h"

class Bus;

class Cartridge
{
public:
  // Mirroring used in the game
  enum class Mirroring
  {
    Horizontal,
    Vertical,
    OneScreen_Lo,
    OneScreen_Hi,
  };

  // Constructor with the path to a *.NES file
  Cartridge(const std::string& file);
  // Destructor
  ~Cartridge() {}

  // Read access from CPU (CPU bus)
  void ReadByCPU(uint16_t addr, uint8_t& data);
  // Write access from CPU (CPU bus)
  void WriteByCPU(uint16_t addr, uint8_t data);

  // Read access from PPU (PPU bus)
  void ReadByPPU(uint16_t addr, uint8_t& data);
  // Write access from PPU (PPU bus)
  void WriteByPPU(uint16_t addr, uint8_t data);

  // Return mirroring mode
  Mirroring GetMirroring() { return m_mirroring; }
  // Check if NES file is valid
  bool IsImageValid() const { return m_imageValid; }

private:
  // Mapper, mirroring, battery, trainer
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

  // Mapper, VS/Playchoice, NES 2.0
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

  // Header of the NES image
  Header m_header;

  // Flag if image is valid
  bool m_imageValid = false;

  // Mirroring mode
  Mirroring m_mirroring = Mirroring::Horizontal;

  // Program ROM
  std::vector<uint8_t> m_PRG_ROM;
  // Character ROM
  std::vector<uint8_t> m_CHR_ROM;
  // Character RAM (used when CHR_ROM is not present)
  std::vector<uint8_t> m_CHR_RAM;

  // ID of the used mapper
  uint8_t m_mapperID = 0;

  // Mapper
  std::unique_ptr<Mapper> m_mapper;
};

