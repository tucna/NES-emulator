#pragma once

#include <array>

#include "Cpu.h"

class Bus
{
public:
  Bus();
  ~Bus() {}

  void Write(uint16_t addr, uint8_t data);

  uint8_t Read(uint16_t addr, bool bReadOnly = false);

  // TODO
  void WriteToRAM(size_t index, uint8_t data);

private:
  // TODO - Fake RAM
  std::array<uint8_t, 64 * 1024> m_ram;

  Cpu* m_cpu;
};

