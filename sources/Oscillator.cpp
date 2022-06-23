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
  }
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
