/*
  Addressing modes are taken from NES emulator of javidx9 - OneLoneCoder.com.
  Thank you for the great work!
*/

#include "Bus.h"
#include "Cpu.h"

Cpu::Cpu(Bus* bus) :
  m_bus(bus),
  m_status(0),
  m_fetched(0),
  m_addrAbs(0),
  m_addrRel(0)
{
}

// Address Mode: Implied
// There is no additional data required for this instruction. The instruction
// does something very simple like like sets a status bit. However, we will
// target the accumulator, for instructions like PHA
uint8_t Cpu::IMP()
{
  m_fetched = m_a;
  return 0;
}

// Address Mode: Immediate
// The instruction expects the next byte to be used as a value, so we'll prep
// the read address to point to the next byte
uint8_t Cpu::IMM()
{
  m_addrAbs = m_programCounter++;
  return 0;
}

// Address Mode: Zero Page
// To save program bytes, zero page addressing allows you to absolutely address
// a location in first 0xFF bytes of address range. Clearly this only requires
// one byte instead of the usual two.
uint8_t Cpu::ZP0()
{
  m_addrAbs = Read(m_programCounter);
  m_programCounter++;
  m_addrAbs &= 0x00FF;
  return 0;
}

// Address Mode: Zero Page with X Offset
// Fundamentally the same as Zero Page addressing, but the contents of the X Register
// is added to the supplied single byte address. This is useful for iterating through
// ranges within the first page.
uint8_t Cpu::ZPX()
{
  m_addrAbs = (Read(m_programCounter) + m_x);
  m_programCounter++;
  m_addrAbs &= 0x00FF;
  return 0;
}

// Address Mode: Zero Page with Y Offset
// Same as above but uses Y Register for offset
uint8_t Cpu::ZPY()
{
  m_addrAbs = (Read(m_programCounter) + m_y);
  m_programCounter++;
  m_addrAbs &= 0x00FF;
  return 0;
}

// Address Mode: Relative
// This address mode is exclusive to branch instructions. The address
// must reside within -128 to +127 of the branch instruction, i.e.
// you cant directly branch to any address in the addressable range.
uint8_t Cpu::REL()
{
  m_addrRel = Read(m_programCounter);
  m_programCounter++;
  if (m_addrRel & 0x80)
    m_addrRel |= 0xFF00;
  return 0;
}

// Address Mode: Absolute
// A full 16-bit address is loaded and used
uint8_t Cpu::ABS()
{
  uint16_t lo = Read(m_programCounter);
  m_programCounter++;
  uint16_t hi = Read(m_programCounter);
  m_programCounter++;

  m_addrAbs = (hi << 8) | lo;

  return 0;
}

// Address Mode: Absolute with X Offset
// Fundamentally the same as absolute addressing, but the contents of the X Register
// is added to the supplied two byte address. If the resulting address changes
// the page, an additional clock cycle is required
uint8_t Cpu::ABX()
{
  uint16_t lo = Read(m_programCounter);
  m_programCounter++;
  uint16_t hi = Read(m_programCounter);
  m_programCounter++;

  m_addrAbs = (hi << 8) | lo;
  m_addrAbs += m_x;

  if ((m_addrAbs & 0xFF00) != (hi << 8))
    return 1;
  else
    return 0;
}

// Address Mode: Absolute with Y Offset
// Fundamentally the same as absolute addressing, but the contents of the Y Register
// is added to the supplied two byte address. If the resulting address changes
// the page, an additional clock cycle is required
uint8_t Cpu::ABY()
{
  uint16_t lo = Read(m_programCounter);
  m_programCounter++;
  uint16_t hi = Read(m_programCounter);
  m_programCounter++;

  m_addrAbs = (hi << 8) | lo;
  m_addrAbs += m_y;

  if ((m_addrAbs & 0xFF00) != (hi << 8))
    return 1;
  else
    return 0;
}

// Address Mode: Indirect
// The supplied 16-bit address is read to get the actual 16-bit address. This is
// instruction is unusual in that it has a bug in the hardware! To emulate its
// function accurately, we also need to emulate this bug. If the low byte of the
// supplied address is 0xFF, then to read the high byte of the actual address
// we need to cross a page boundary. This doesnt actually work on the chip as
// designed, instead it wraps back around in the same page, yielding an
// invalid actual address
uint8_t Cpu::IND()
{
  uint16_t ptr_lo = Read(m_programCounter);
  m_programCounter++;
  uint16_t ptr_hi = Read(m_programCounter);
  m_programCounter++;

  uint16_t ptr = (ptr_hi << 8) | ptr_lo;

  if (ptr_lo == 0x00FF) // Simulate page boundary hardware bug
  {
    m_addrAbs = (Read(ptr & 0xFF00) << 8) | Read(ptr + 0);
  }
  else // Behave normally
  {
    m_addrAbs = (Read(ptr + 1) << 8) | Read(ptr + 0);
  }

  return 0;
}

// Address Mode: Indirect X
// The supplied 8-bit address is offset by X Register to index
// a location in page 0x00. The actual 16-bit address is read
// from this location
uint8_t Cpu::IZX()
{
  uint16_t t = Read(m_programCounter);
  m_programCounter++;

  uint16_t lo = Read((uint16_t)(t + (uint16_t)m_x) & 0x00FF);
  uint16_t hi = Read((uint16_t)(t + (uint16_t)m_x + 1) & 0x00FF);

  m_addrAbs = (hi << 8) | lo;

  return 0;
}

// Address Mode: Indirect Y
// The supplied 8-bit address indexes a location in page 0x00. From
// here the actual 16-bit address is read, and the contents of
// Y Register is added to it to offset it. If the offset causes a
// change in page then an additional clock cycle is required.
uint8_t Cpu::IZY()
{
  uint16_t t = Read(m_programCounter);
  m_programCounter++;

  uint16_t lo = Read(t & 0x00FF);
  uint16_t hi = Read((t + 1) & 0x00FF);

  m_addrAbs = (hi << 8) | lo;
  m_addrAbs += m_y;

  if ((m_addrAbs & 0xFF00) != (hi << 8))
    return 1;
  else
    return 0;
}

void Cpu::Write(uint16_t addr, uint8_t data)
{
  m_bus->Write(addr, data);
}

uint8_t Cpu::Read(uint16_t addr)
{
  return m_bus->Read(addr, false);
}
