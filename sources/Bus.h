/*
  Addressing modes, lookup table and official instructions are taken from NES emulator of javidx9 - OneLoneCoder.com.
  Thank you for the great work!
*/

#pragma once

#include <array>

class Cartridge;
class Cpu;
class Ppu;

class Bus
{
public:
  Bus() {}
  ~Bus() {}

  void Write(uint16_t addr, uint8_t data);
  uint8_t Read(uint16_t addr, bool bReadOnly = false);

  void ConnectRam(std::array<uint8_t, 2 * 1024>* ram);
  void ConnectCpu(Cpu* cpu);
  void ConnectPpu(Ppu* ppu);
  void ConnectCartridge(Cartridge* cartridge);

  uint8_t& GetController1() { return m_controller[0]; }

private:
  std::array<uint8_t, 2 * 1024>* m_ram;

  // Connected controllers
  // TODO move controller to a separated structure
  uint8_t m_controller[2];
  uint8_t m_controllerState[2];

  Cpu* m_cpu;
  Ppu* m_ppu;
  Cartridge* m_cartridge;
};

