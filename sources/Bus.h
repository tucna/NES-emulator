/*
  Addressing modes, lookup table and official instructions are taken from NES emulator of javidx9 - OneLoneCoder.com.
  Thank you for the great work!
*/

#pragma once

#include <array>

#include "Common.h"

class Cartridge;
class CPU;
class PPU;

class Bus
{
public:
  Bus() {}
  ~Bus() {}

  void Write(uint16_t addr, uint8_t data);
  uint8_t Read(uint16_t addr, bool bReadOnly = false);

  void ConnectRam(RAM2KB* ram);
  void ConnectCpu(CPU* cpu);
  void ConnectPPU(PPU* ppu);
  void ConnectCartridge(Cartridge* cartridge);

  uint8_t& GetController1() { return m_controller[0]; }

private:
  RAM2KB* m_ram;

  // Connected controllers
  // TODO move controller to a separated structure
  uint8_t m_controller[2];
  uint8_t m_controllerState[2];

  CPU* m_cpu;
  PPU* m_ppu;
  Cartridge* m_cartridge;
};

