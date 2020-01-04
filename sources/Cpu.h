#pragma once

#include <cstdint>

// NES uses 6502
class Cpu
{
public:
  Cpu();
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

  // Capture internal CPU status
  uint8_t m_status;
};

