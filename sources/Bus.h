#pragma once

#include <array>

class Cartridge;
class Cpu;
class Ppu;

class Bus
{
public:
  Bus();
  ~Bus() {}

  void WriteCpu(uint16_t addr, uint8_t data);
  uint8_t ReadCpu(uint16_t addr, bool bReadOnly = false);

  void ConnectCpu(Cpu* cpu);
  void ConnectPpu(Ppu* ppu);
  void ConnectCartridge(Cartridge* cartridge);

  // TODO
  void Clock();

private:
  // TODO - Fake RAM
  std::array<uint8_t, 2 * 1024> m_ram;

  Cpu* m_cpu;
  Ppu* m_ppu;
  Cartridge* m_cartridge;

  uint32_t m_systemClockCounter;
};

