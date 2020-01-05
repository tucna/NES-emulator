#pragma once

#include <map>
#include <memory>

#include "Bus.h"
#include "Cartridge.h"
#include "Cpu.h"
#include "Ppu.h"

class NES
{
public:
  NES();
  ~NES() {}

  Bus& GetBus() { return *m_bus; }
  Cpu& GetCpu() { return *m_cpu; }
  Ppu& GetPpu() { return *m_ppu; }

  const std::map<uint16_t, std::string>& GetAssembly() { return m_asm; }

private:
  void InitExample();
  void InsertCartridge(const std::string& file);

  // Hardware
  std::unique_ptr<Bus> m_bus;
  std::unique_ptr<Cpu> m_cpu;
  std::unique_ptr<Ppu> m_ppu;
  std::unique_ptr<Cartridge> m_cartridge; // Not so logical because NES does not "own" the cartridge, but it is simpler

  std::map<uint16_t, std::string> m_asm;
};

