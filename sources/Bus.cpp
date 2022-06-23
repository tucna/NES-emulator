#include "Bus.h"
#include "Cartridge.h"
#include "CPU.h"
#include "PPU.h"

void Bus::Write(uint16_t addr, uint8_t data)
{
  if (addr >= 0x0000 && addr <= 0x1FFF) // Internal RAM + 4 times mirroring
  {
    (*m_ram)[addr & 0x07FF] = data;
  }
  else if (addr >= 0x2000 && addr <= 0x3FFF) // PPU registers + mirroring
  {
    // PPU Address range, mirrored every 8
    m_ppu->WriteByCPU(addr & 0x0007, data);
  }
  else if (addr >= 0x4016 && addr <= 0x4017)
  {
    m_controllerState[addr & 0x0001] = m_controller[addr & 0x0001];
  }
}

uint8_t Bus::Read(uint16_t addr, bool bReadOnly)
{
  uint8_t data = 0x00;

  if (addr >= 0x0000 && addr <= 0x1FFF) // Internal RAM + 4 times mirroring
  {
    data = (*m_ram)[addr & 0x07FF];
  }
  else if (addr >= 0x2000 && addr <= 0x3FFF) // PPU registers + mirroring
  {
    // PPU Address range, mirrored every 8
    data = m_ppu->ReadByCPU(addr & 0x0007, bReadOnly);
  }
  else if (addr >= 0x4016 && addr <= 0x4017)
  {
    data = (m_controllerState[addr & 0x0001] & 0x80) > 0;
    m_controllerState[addr & 0x0001] <<= 1;
  }
  else if (addr >= 0x4020 && addr <= 0xFFFF) // Cartridge
  {
    // Not sure if first address should not be 0x8000 to skip the header bits
    m_cartridge->ReadByCPU(addr, data);
  }

  return data;
}

void Bus::ConnectRam(std::array<uint8_t, 2 * 1024>* ram)
{
  m_ram = ram;
}

void Bus::ConnectCpu(CPU* cpu)
{
  m_cpu = cpu;
  m_cpu->ConnectToBus(this);
}

void Bus::ConnectPPU(PPU* ppu)
{
  m_ppu = ppu;
}

void Bus::ConnectCartridge(Cartridge* cartridge)
{
  m_cartridge = cartridge;
  m_cartridge->ConnectToBus(this);
}
