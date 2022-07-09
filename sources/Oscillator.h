#pragma once

#include <cstdint>

class CPU; // TODO rename to CPU and PPU
class PPU;

// Hardware to generate clock signal
class Oscillator
{
public:
  // Constructor
  Oscillator(CPU* cpu, PPU* ppu);
  // Destructor
  ~Oscillator();

  // Input signal in a form of time step
  void Input(float timeStep);
  // Provide enough ticks to execute instruction
  void TicksForCurrentInstruction();
  // Provide enough ticks to draw whole frame
  void TicksForCurrentFrame();
  // Reset
  void Reset();

private:
  // Tick for CPU and PPU
  void Tick();

  CPU* m_cpu;
  PPU* m_ppu;

  float m_residualTime = 0;

  uint32_t m_systemClockCounter = 0;
};

