#pragma once

#include <memory>

#include "Bus.h"
#include "Cpu.h"

class NES
{
public:
  NES();
  ~NES() {}

  const Bus& GetBus() { return *m_bus; }
  const Cpu& GetCpu() { return *m_cpu; }

private:
  std::unique_ptr<Bus> m_bus;
  std::unique_ptr<Cpu> m_cpu;
};

