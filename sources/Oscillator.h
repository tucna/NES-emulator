#pragma once

#include <cstdint>

class Cpu; // TODO rename to CPU and PPU
class Ppu;

// Hardware to generate clock signal
class Oscillator
{
public:
  Oscillator(Cpu* cpu, Ppu* ppu);
  ~Oscillator();

  void Input(float timeStep);

private:
  void Tick();

  Cpu* m_cpu;
  Ppu* m_ppu;

  float m_residualTime = 0;

  uint32_t m_systemClockCounter = 0;
};

