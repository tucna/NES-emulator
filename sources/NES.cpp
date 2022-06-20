#include <sstream>

#include "NES.h"

using namespace std;

NES::NES() :
  m_systemClockCounter(0)
{
  m_bus = make_unique<Bus>();
  m_cpu = make_unique<Cpu>();
  m_ppu = make_unique<Ppu>();

  m_bus->ConnectCpu(m_cpu.get());
  m_bus->ConnectPpu(m_ppu.get());
  m_bus->ConnectRam(&m_ram);

  InsertCartridge("roms/cpu/nestest.nes");
}

void NES::Clock()
{
  m_ppu->Clock();

  if (m_systemClockCounter % 3 == 0)
  {
    m_cpu->Clock();
  }

  if (m_ppu->NMI())
  {
    m_ppu->NMIHandled();
    m_cpu->NMI();
  }

  m_systemClockCounter++;
}

void NES::InsertCartridge(const std::string& file)
{
  // Load the cartridge
  m_cartridge = std::make_unique<Cartridge>(file);
  m_bus->ConnectCartridge(m_cartridge.get());
  m_ppu->ConnectCartridge(m_cartridge.get());

  if (!m_cartridge->IsImageValid())
  {
    // TODO
    return;
  }

  // Extract dissassembly
  m_asm = m_cpu->Disassemble(0x0000, 0xFFFF);

  // Reset
  m_cpu->Reset();
}
