#include <fstream>

#include "Cartridge.h"
#include "Mapper_000.h"

Cartridge::Cartridge(const std::string & file) :
  m_imageValid(false),
  m_mapperID(0),
  m_mirror(Mirror::Horizontal),
  m_bus(nullptr)
{
  struct Header
  {
    char name[4];
    uint8_t prgRomChunks;
    uint8_t chrRomChunks;
    uint8_t mapper1;
    uint8_t mapper2;
    uint8_t prgRamSize;
    uint8_t tvSystem1;
    uint8_t tvSystem2;
    char unused[5];
  } header;

  m_imageValid = false;

  std::ifstream ifs;
  ifs.open(file, std::ifstream::binary);
  if (ifs.is_open())
  {
    // Read file header
    ifs.read((char*)&header, sizeof(Header));

    // If a "trainer" exists we just need to read past
    // it before we get to the good stuff
    if (header.mapper1 & 0x04)
      ifs.seekg(512, std::ios_base::cur);

    // Determine Mapper ID
    m_mapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
    m_mirror = (header.mapper1 & 0x01) ? Mirror::Vertical : Mirror::Horizontal;

    // "Discover" File Format
    uint8_t fileType = 1;

    if (fileType == 0)
    {
      // TODO
    }

    if (fileType == 1)
    {
      m_PRGBanks = header.prgRomChunks;
      m_PRGMemory.resize(m_PRGBanks * 16384); // 2MB
      ifs.read((char*)m_PRGMemory.data(), m_PRGMemory.size());

      m_CHRBanks = header.chrRomChunks;
      m_CHRMemory.resize(m_CHRBanks * 8192); // 1MB
      ifs.read((char*)m_CHRMemory.data(), m_CHRMemory.size());
    }

    if (fileType == 2)
    {
      // TODO
    }

    // Load appropriate mapper
    switch (m_mapperID)
    {
    case 0: m_mapper = std::make_unique<Mapper_000>(m_PRGBanks, m_CHRBanks); break;
    }

    m_imageValid = true;
    ifs.close();
  }
}

void Cartridge::ReadByCPU(uint16_t addr, uint8_t& data)
{
  uint32_t mappedAddr = 0;

  m_mapper->MapReadByCPU(addr, mappedAddr);
  data = m_PRGMemory[mappedAddr];
}

void Cartridge::WriteByCPU(uint16_t addr, uint8_t data)
{
  uint32_t mappedAddr = 0;

  m_mapper->MapWriteByCPU(addr, mappedAddr);
  m_PRGMemory[mappedAddr] = data;
}

void Cartridge::ReadByPPU(uint16_t addr, uint8_t& data)
{
  uint32_t mappedAddr = 0;

  m_mapper->MapReadByPPU(addr, mappedAddr);
  data = m_CHRMemory[mappedAddr];
}

void Cartridge::WriteByPPU(uint16_t addr, uint8_t data)
{
  uint32_t mappedAddr = 0;

  m_mapper->MapWriteByPPU(addr, mappedAddr);
  m_CHRMemory[mappedAddr] = data;
}
