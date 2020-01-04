#pragma once

#include <cstdint>
#include <string>

class Bus;

// NES uses 6502
class Cpu
{
public:
  struct Instruction
  {
    std::string name;
    uint8_t(Cpu::*operate)(void);
    uint8_t(Cpu::*addrmode)(void);
    uint8_t cycles;
  };

  Cpu(Bus* bus);
  ~Cpu() {}

  bool GetB0() const { return m_status & Flags::C; }
  bool GetB1() const { return m_status & Flags::Z; }
  bool GetB2() const { return m_status & Flags::I; }
  bool GetB3() const { return m_status & Flags::D; }
  bool GetB4() const { return m_status & Flags::B; }
  bool GetB5() const { return m_status & Flags::U; }
  bool GetB6() const { return m_status & Flags::V; }
  bool GetB7() const { return m_status & Flags::N; }

private:
  enum Flags
  {
    C = (1 << 0),	// Carry Bit
    Z = (1 << 1),	// Zero
    I = (1 << 2),	// Disable Interrupts
    D = (1 << 3),	// Decimal Mode (unused in this implementation)
    B = (1 << 4),	// Break
    U = (1 << 5),	// Unused
    V = (1 << 6),	// Overflow
    N = (1 << 7),	// Negative
  };

  // Addressing modes
  uint8_t IMP();	uint8_t IMM();
  uint8_t ZP0();	uint8_t ZPX();
  uint8_t ZPY();	uint8_t REL();
  uint8_t ABS();	uint8_t ABX();
  uint8_t ABY();	uint8_t IND();
  uint8_t IZX();	uint8_t IZY();

  void Write(uint16_t addr, uint8_t data);
  uint8_t Read(uint16_t addr);

  // Registers
  uint8_t m_a;
  uint8_t m_x;
  uint8_t m_y;
  uint8_t m_stackPointer;
  uint16_t m_programCounter;

  // Capture internal CPU status
  uint8_t m_status;

  uint8_t m_fetched;
  uint16_t m_addrAbs;
  uint16_t m_addrRel;

  // Bus which the CPU is connected to
  Bus* m_bus;
};

