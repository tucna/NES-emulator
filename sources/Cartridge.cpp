#include <fstream>
#include <sstream>

#include "Cartridge.h"
#include "Mapper_000.h"

Cartridge::Cartridge(const std::string& file)
{
  if (file.size() == 0) // debug cartridge
  {
    m_PRG_ROM.resize(0xBFE0);
    size_t offset = 0;

    std::stringstream ss;
    ss << "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA 4C 19 80";

    while (!ss.eof())
    {
      std::string b;
      ss >> b;
      m_PRG_ROM[offset++] = (uint8_t)std::stoul(b, nullptr, 16);
    }

    m_mapper = std::make_unique<Mapper_000>(m_header.prgRomBanks, m_header.chrRomBanks);
    m_CHR_RAM.resize(8192, 0); // 8B
  }
  else
  {
    m_imageValid = false;

    std::ifstream ifs;
    ifs.open(file, std::ifstream::binary);

    if (ifs.is_open())
    {
      // Read file header
      ifs.read((char*)&m_header, sizeof(Header));

      // If a "trainer" exists we just need to read past
      // it before we get to the good stuff
      if (m_header.flags6.trainer)
        ifs.seekg(512, std::ios_base::cur);

      // Determine Mapper ID
      m_mapperID = (m_header.flags7.higher_nibble_mapper_ID << 4) | m_header.flags6.lower_nibble_mapper_ID;
      m_mirroring = m_header.flags6.mirroring ? Mirroring::Vertical : Mirroring::Horizontal;

      // TODO correct variant handling
      uint8_t fileType = 1;

      if (fileType == 0)
      {
        // TODO
        __debugbreak();
      }

      // If byte 7 AND $0C = $00, and bytes 12 - 15 are all 0, then iNES
      if (fileType == 1)
      {
        m_PRG_ROM.resize(m_header.prgRomBanks * 16384); // 16kB
        ifs.read((char*)m_PRG_ROM.data(), m_PRG_ROM.size());

        if (m_header.chrRomBanks > 0)
        {
          m_CHR_ROM.resize(m_header.chrRomBanks * 8192); // 8B
          ifs.read((char*)m_CHR_ROM.data(), m_CHR_ROM.size());
        }
        else // if there is 0 prg rom banks in use, use RAM instead
        {
          m_CHR_RAM.resize(8192); // 8B
        }
      }

      if (fileType == 2)
      {
        // TODO
        __debugbreak();
      }

      // Load appropriate mapper
      switch (m_mapperID)
      {
        case 0:
          m_mapper = std::make_unique<Mapper_000>(m_header.prgRomBanks, m_header.chrRomBanks);
          break;
        default:
          __debugbreak(); // Not implemented
      }

      m_imageValid = true;

      ifs.close();
    }
  }
}

void Cartridge::ReadByCPU(uint16_t addr, uint8_t& data)
{
  uint32_t mappedAddr = 0;

  m_mapper->MapReadByCPU(addr, mappedAddr);
  data = m_PRG_ROM[mappedAddr];
}

void Cartridge::WriteByCPU(uint16_t addr, uint8_t data)
{
  uint32_t mappedAddr = 0;

  m_mapper->MapWriteByCPU(addr, mappedAddr);
  m_PRG_ROM[mappedAddr] = data;
}

void Cartridge::ReadByPPU(uint16_t addr, uint8_t& data)
{
  uint32_t mappedAddr = 0;

  m_mapper->MapReadByPPU(addr, mappedAddr);

  if (m_header.chrRomBanks == 0)
    data = m_CHR_RAM[mappedAddr];
  else
    data = m_CHR_ROM[mappedAddr];
}

void Cartridge::WriteByPPU(uint16_t addr, uint8_t data)
{
  uint32_t mappedAddr = 0;

  m_mapper->MapWriteByPPU(addr, mappedAddr);

  if (m_header.chrRomBanks == 0)
    m_CHR_RAM[mappedAddr] = data;
  else
    m_CHR_ROM[mappedAddr] = data;
}
