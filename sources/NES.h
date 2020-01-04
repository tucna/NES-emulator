#pragma once

#include <memory>

#include "Bus.h"
#include "Cpu.h"

class NES
{
public:
  NES();
  ~NES() {}

private:
  std::unique_ptr<Bus> m_bus;
  std::unique_ptr<Cpu> m_cpu;
};

