#include "Bus.h"
#include "Cartridge.h"
#include "Cpu.h"
#include "Ppu.h"

Bus::Bus() :
  m_systemClockCounter(0)
{
}

void Bus::WriteCpu(uint16_t addr, uint8_t data)
{
  if (addr >= 0x0000 && addr <= 0x1FFF) // Internal RAM + 4 times mirroring
  {
    m_ram[addr & 0x07FF] = data;
  }
  else if (addr >= 0x2000 && addr <= 0x3FFF) // PPU registers + mirroring
  {
    // PPU Address range, mirrored every 8
    //m_ppu->cpuWrite(addr & 0x0007, data); TODO
  }
  else if (addr >= 0x4000 && addr <= 0x401F) // APU handling
  {
    // APU
  }
  else if (addr >= 0x4020 && addr <= 0xFFFF) // Cartridge  //m_cartridge->cpuRead(addr, data))
  {
    // Cartridge Address Range
  }
}


uint8_t Bus::ReadCpu(uint16_t addr, bool bReadOnly)
{
  uint8_t data = 0x00;

  if (addr >= 0x0000 && addr <= 0x1FFF) // Internal RAM + 4 times mirroring
  {
    data = m_ram[addr & 0x07FF];
  }
  else if (addr >= 0x2000 && addr <= 0x3FFF) // PPU registers + mirroring
  {
    // PPU Address range, mirrored every 8
    //data = m_ppu->cpuRead(addr & 0x0007, bReadOnly); TODO
  }
  else if (addr >= 0x4000 && addr <= 0x401F) // APU handling
  {
    // APU
  }
  else if (addr >= 0x4020 && addr <= 0xFFFF) // Cartridge
  {
    // Not sure if first address should not be 0x8000 to skip the header bits
    m_cartridge->ReadByCPU(addr, data);
  }

  return data;
}

void Bus::ConnectCpu(Cpu * cpu)
{
  m_cpu = cpu;
  m_cpu->ConnectToBus(this);
}

void Bus::ConnectPpu(Ppu * ppu)
{
  m_ppu = ppu;
  m_ppu->ConnectToBus(this);
}

void Bus::ConnectCartridge(Cartridge * cartridge)
{
  m_cartridge = cartridge;
  m_cartridge->ConnectToBus(this);
}

void Bus::WriteToRAM(size_t index, uint8_t data)
{
  m_ram[index] = data;
}

void Bus::Clock()
{
  m_ppu->Clock();

  if (m_systemClockCounter % 3 == 0)
  {
    m_cpu->Clock();
  }

  m_systemClockCounter++;
}
