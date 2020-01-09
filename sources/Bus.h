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

  void Write(uint16_t addr, uint8_t data);
  uint8_t Read(uint16_t addr, bool bReadOnly = false);

  void ConnectRam(std::array<uint8_t, 2 * 1024>* ram);

  void ConnectCpu(Cpu* cpu);
  void ConnectPpu(Ppu* ppu);
  void ConnectCartridge(Cartridge* cartridge);

  // TODO
  //void Clock();

private:
  std::array<uint8_t, 2 * 1024>* m_ram;

  Cpu* m_cpu;
  Ppu* m_ppu;
  Cartridge* m_cartridge;
};

