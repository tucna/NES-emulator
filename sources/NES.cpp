#include <sstream>

#include "NES.h"

using namespace std;

NES::NES()
{
  m_bus = make_unique<Bus>();
  m_cpu = make_unique<Cpu>();
  m_ppu = make_unique<Ppu>();

  m_bus->ConnectCpu(m_cpu.get());
  m_bus->ConnectPpu(m_ppu.get());

  // TODO: some switch here?
  //InitExample();
  InsertCartridge("roms/nestest.nes");
}

void NES::InitExample()
{
  // Load Program (assembled at https://www.masswerk.at/6502/assembler.html)
  /*
    *=$8000
    LDX #10
    STX $0000
    LDX #3
    STX $0001
    LDY $0000
    LDA #0
    CLC
    loop
    ADC $0001
    DEY
    BNE loop
    STA $0002
    NOP
    NOP
    NOP
  */

  // Convert hex string into bytes for RAM
  std::stringstream ss;
  ss << "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA";
  uint16_t nOffset = 0x8000;

  while (!ss.eof())
  {
    std::string b;
    ss >> b;

    m_bus->WriteToRAM(nOffset++, (uint8_t)std::stoul(b, nullptr, 16));
  }

  // Set Reset Vector
  m_bus->WriteToRAM(0xFFFC, 0x00);
  m_bus->WriteToRAM(0xFFFD, 0x80);

  // Extract dissassembly
  m_asm = m_cpu->Disassemble(0x0000, 0xFFFF);

  // Reset
  m_cpu->Reset();
}

void NES::InsertCartridge(const std::string& file)
{
  // Load the cartridge
  m_cartridge = std::make_unique<Cartridge>(file);

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
