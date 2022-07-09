#include "CPU.h"
#include "Oscillator.h"
#include "PPU.h"

Oscillator::Oscillator(CPU* cpu, PPU* ppu) :
  m_cpu(cpu),
  m_ppu(ppu)
{
}

Oscillator::~Oscillator()
{
}

void Oscillator::Input(float timeStep)
{
  if (m_residualTime > 0.0f)
  {
    m_residualTime -= timeStep;
  }
  else
  {
    m_residualTime += (1.0f / 60.0f) - timeStep;

    do
    {
      Tick();
    }
    while (!m_ppu->IsFrameCompleted());

    // Reset frame completion flag
    m_ppu->SetFrameNotCompleted();
  }
}

void Oscillator::TicksForCurrentInstruction()
{
  // Clock enough times to execute a whole CPU instruction
  do { Tick(); } while (!m_cpu->IsInstructionCompleted());
  // CPU clock runs slower than system clock, so it may be
  // still need some ticks to finish PPU run.
  do { Tick(); } while (m_cpu->IsInstructionCompleted());
}

void Oscillator::TicksForCurrentFrame()
{
  // Clock enough times to draw a single frame
  do { Tick(); } while (!m_ppu->IsFrameCompleted());
  // Use residual clock cycles to complete current instruction
  do { Tick(); } while (!m_cpu->IsInstructionCompleted());
  // Reset frame completion flag
  m_ppu->SetFrameNotCompleted();
}

void Oscillator::Reset()
{
  m_residualTime = 0;
  m_systemClockCounter = 0;
}

void Oscillator::Tick()
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
