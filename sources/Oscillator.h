#pragma once

#include <cstdint>

class CPU; // TODO rename to CPU and PPU
class PPU;

// Hardware to generate clock signal
class Oscillator
{
public:
  Oscillator(CPU* cpu, PPU* ppu);
  ~Oscillator();

  void Input(float timeStep);
  void TicksForCurrentInstruction();
  void TicksForCurrentFrame();

private:
  void Tick();

  CPU* m_cpu;
  PPU* m_ppu;

  float m_residualTime = 0;

  uint32_t m_systemClockCounter = 0;
};

