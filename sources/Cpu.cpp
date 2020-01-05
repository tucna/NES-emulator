/*
  Addressing modes and instructions are taken from NES emulator of javidx9 - OneLoneCoder.com.
  Thank you for the great work!
*/

#include "Bus.h"
#include "Cpu.h"

Cpu::Cpu(Bus* bus) :
  m_bus(bus),
  m_status(0),
  m_fetched(0),
  m_addrAbs(0),
  m_addrRel(0),
  m_opcode(0),
  m_temp(0),
  m_cycles(0),
  m_clockCount(0)
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

uint8_t Cpu::Fetch()
{
  if (!(m_lookup[m_opcode].addrmode == &Cpu::IMP))
    m_fetched = Read(m_addrAbs);

  return m_fetched;
}

uint8_t Cpu::GetFlag(Flags flag)
{
  return ((m_status & flag) > 0) ? 1 : 0;
}

void Cpu::SetFlag(Flags flag, bool setClear)
{
  if (setClear)
    m_status |= flag;
  else
    m_status &= ~flag;
}

// Instruction: Add with Carry In
// Function:    A = A + M + C
// Flags Out:   C, V, N, Z
//
// Explanation:
// The purpose of this function is to add a value to the accumulator and a carry bit. If
// the result is > 255 there is an overflow setting the carry bit. Ths allows you to
// chain together ADC instructions to add numbers larger than 8-bits. This in itself is
// simple, however the 6502 supports the concepts of Negativity/Positivity and Signed Overflow.
//
// 10000100 = 128 + 4 = 132 in normal circumstances, we know this as unsigned and it allows
// us to represent numbers between 0 and 255 (given 8 bits). The 6502 can also interpret 
// this word as something else if we assume those 8 bits represent the range -128 to +127,
// i.e. it has become signed.
//
// Since 132 > 127, it effectively wraps around, through -128, to -124. This wraparound is
// called overflow, and this is a useful to know as it indicates that the calculation has
// gone outside the permissable range, and therefore no longer makes numeric sense.
//
// Note the implementation of ADD is the same in binary, this is just about how the numbers
// are represented, so the word 10000100 can be both -124 and 132 depending upon the 
// context the programming is using it in. We can prove this!
//
//  10000100 =  132  or  -124
// +00010001 = + 17      + 17
//  ========    ===       ===     See, both are valid additions, but our interpretation of
//  10010101 =  149  or  -107     the context changes the value, not the hardware!
//
// In principle under the -128 to 127 range:
// 10000000 = -128, 11111111 = -1, 00000000 = 0, 00000000 = +1, 01111111 = +127
// therefore negative numbers have the most significant set, positive numbers do not
//
// To assist us, the 6502 can set the overflow flag, if the result of the addition has
// wrapped around. V <- ~(A^M) & A^(A+M+C) :D lol, let's work out why!
//
// Let's suppose we have A = 30, M = 10 and C = 0
//          A = 30 = 00011110
//          M = 10 = 00001010+
//     RESULT = 40 = 00101000
//
// Here we have not gone out of range. The resulting significant bit has not changed.
// So let's make a truth table to understand when overflow has occurred. Here I take
// the MSB of each component, where R is RESULT.
//
// A  M  R | V | A^R | A^M |~(A^M) |
// 0  0  0 | 0 |  0  |  0  |   1   |
// 0  0  1 | 1 |  1  |  0  |   1   |
// 0  1  0 | 0 |  0  |  1  |   0   |
// 0  1  1 | 0 |  1  |  1  |   0   |  so V = ~(A^M) & (A^R)
// 1  0  0 | 0 |  1  |  1  |   0   |
// 1  0  1 | 0 |  0  |  1  |   0   |
// 1  1  0 | 1 |  1  |  0  |   1   |
// 1  1  1 | 0 |  0  |  0  |   1   |
//
// We can see how the above equation calculates V, based on A, M and R. V was chosen
// based on the following hypothesis:
//       Positive Number + Positive Number = Negative Result -> Overflow
//       Negative Number + Negative Number = Positive Result -> Overflow
//       Positive Number + Negative Number = Either Result -> Cannot Overflow
//       Positive Number + Positive Number = Positive Result -> OK! No Overflow
//       Negative Number + Negative Number = Negative Result -> OK! NO Overflow
uint8_t Cpu::ADC()
{
  // Grab the data that we are adding to the accumulator
  Fetch();

  // Add is performed in 16-bit domain for emulation to capture any
  // carry bit, which will exist in bit 8 of the 16-bit word
  m_temp = (uint16_t)m_a + (uint16_t)m_fetched + (uint16_t)GetFlag(C);

  // The carry flag out exists in the high byte bit 0
  SetFlag(C, m_temp > 255);

  // The Zero flag is set if the result is 0
  SetFlag(Z, (m_temp & 0x00FF) == 0);

  // The signed Overflow flag is set based on all that up there! :D
  SetFlag(V, (~((uint16_t)m_a ^ (uint16_t)m_fetched) & ((uint16_t)m_a ^ (uint16_t)m_temp)) & 0x0080);

  // The negative flag is set to the most significant bit of the result
  SetFlag(N, m_temp & 0x80);

  // Load the result into the accumulator (it's 8-bit dont forget!)
  m_a = m_temp & 0x00FF;

  // This instruction has the potential to require an additional clock cycle
  return 1;
}

// Instruction: Subtraction with Borrow In
// Function:    A = A - M - (1 - C)
// Flags Out:   C, V, N, Z
//
// Explanation:
// Given the explanation for ADC above, we can reorganise our data
// to use the same computation for addition, for subtraction by multiplying
// the data by -1, i.e. make it negative
//
// A = A - M - (1 - C)  ->  A = A + -1 * (M - (1 - C))  ->  A = A + (-M + 1 + C)
//
// To make a signed positive number negative, we can invert the bits and add 1
// (OK, I lied, a little bit of 1 and 2s complement :P)
//
//  5 = 00000101
// -5 = 11111010 + 00000001 = 11111011 (or 251 in our 0 to 255 range)
//
// The range is actually unimportant, because if I take the value 15, and add 251
// to it, given we wrap around at 256, the result is 10, so it has effectively
// subtracted 5, which was the original intention. (15 + 251) % 256 = 10
//
// Note that the equation above used (1-C), but this got converted to + 1 + C.
// This means we already have the +1, so all we need to do is invert the bits
// of M, the data(!) therfore we can simply add, exactly the same way we did
// before.
uint8_t Cpu::SBC()
{
  Fetch();

  // Operating in 16-bit domain to capture carry out

  // We can invert the bottom 8 bits with bitwise xor
  uint16_t value = ((uint16_t)m_fetched) ^ 0x00FF;

  // Notice this is exactly the same as addition from here!
  m_temp = (uint16_t)m_a + value + (uint16_t)GetFlag(C);
  SetFlag(C, m_temp & 0xFF00);
  SetFlag(Z, ((m_temp & 0x00FF) == 0));
  SetFlag(V, (m_temp ^ (uint16_t)m_a) & (m_temp ^ value) & 0x0080);
  SetFlag(N, m_temp & 0x0080);
  m_a = m_temp & 0x00FF;
  return 1;
}

// OK! Complicated operations are done! the following are much simpler
// and conventional. The typical order of events is:
// 1) Fetch the data you are working with
// 2) Perform calculation
// 3) Store the result in desired place
// 4) Set Flags of the status register
// 5) Return if instruction has potential to require additional
//    clock cycle

// Instruction: Bitwise Logic AND
// Function:    A = A & M
// Flags Out:   N, Z
uint8_t Cpu::AND()
{
  Fetch();
  m_a = m_a & m_fetched;
  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & 0x80);
  return 1;
}

// Instruction: Arithmetic Shift Left
// Function:    A = C <- (A << 1) <- 0
// Flags Out:   N, Z, C
uint8_t Cpu::ASL()
{
  Fetch();
  m_temp = (uint16_t)m_fetched << 1;
  SetFlag(C, (m_temp & 0xFF00) > 0);
  SetFlag(Z, (m_temp & 0x00FF) == 0x00);
  SetFlag(N, m_temp & 0x80);
  if (m_lookup[m_opcode].addrmode == &Cpu::IMP)
    m_a = m_temp & 0x00FF;
  else
    Write(m_addrAbs, m_temp & 0x00FF);
  return 0;
}

// Instruction: Branch if Carry Clear
// Function:    if(C == 0) pc = address
uint8_t Cpu::BCC()
{
  if (GetFlag(C) == 0)
  {
    m_cycles++;
    m_addrAbs = m_programCounter + m_addrRel;

    if ((m_addrAbs & 0xFF00) != (m_programCounter & 0xFF00))
      m_cycles++;

    m_programCounter = m_addrAbs;
  }
  return 0;
}

// Instruction: Branch if Carry Set
// Function:    if(C == 1) pc = address
uint8_t Cpu::BCS()
{
  if (GetFlag(C) == 1)
  {
    m_cycles++;
    m_addrAbs = m_programCounter + m_addrRel;

    if ((m_addrAbs & 0xFF00) != (m_programCounter & 0xFF00))
      m_cycles++;

    m_programCounter = m_addrAbs;
  }
  return 0;
}

// Instruction: Branch if Equal
// Function:    if(Z == 1) pc = address
uint8_t Cpu::BEQ()
{
  if (GetFlag(Z) == 1)
  {
    m_cycles++;
    m_addrAbs = m_programCounter + m_addrRel;

    if ((m_addrAbs & 0xFF00) != (m_programCounter & 0xFF00))
      m_cycles++;

    m_programCounter = m_addrAbs;
  }
  return 0;
}

uint8_t Cpu::BIT()
{
  Fetch();
  m_temp = m_a & m_fetched;
  SetFlag(Z, (m_temp & 0x00FF) == 0x00);
  SetFlag(N, m_fetched & (1 << 7));
  SetFlag(V, m_fetched & (1 << 6));
  return 0;
}

// Instruction: Branch if Negative
// Function:    if(N == 1) pc = address
uint8_t Cpu::BMI()
{
  if (GetFlag(N) == 1)
  {
    m_cycles++;
    m_addrAbs = m_programCounter + m_addrRel;

    if ((m_addrAbs & 0xFF00) != (m_programCounter & 0xFF00))
      m_cycles++;

    m_programCounter = m_addrAbs;
  }
  return 0;
}

// Instruction: Branch if Not Equal
// Function:    if(Z == 0) pc = address
uint8_t Cpu::BNE()
{
  if (GetFlag(Z) == 0)
  {
    m_cycles++;
    m_addrAbs = m_programCounter + m_addrRel;

    if ((m_addrAbs & 0xFF00) != (m_programCounter & 0xFF00))
      m_cycles++;

    m_programCounter = m_addrAbs;
  }
  return 0;
}

// Instruction: Branch if Positive
// Function:    if(N == 0) pc = address
uint8_t Cpu::BPL()
{
  if (GetFlag(N) == 0)
  {
    m_cycles++;
    m_addrAbs = m_programCounter + m_addrRel;

    if ((m_addrAbs & 0xFF00) != (m_programCounter & 0xFF00))
      m_cycles++;

    m_programCounter = m_addrAbs;
  }
  return 0;
}

// Instruction: Break
// Function:    Program Sourced Interrupt
uint8_t Cpu::BRK()
{
  m_programCounter++;

  SetFlag(I, 1);
  Write(0x0100 + m_stackPointer, (m_programCounter >> 8) & 0x00FF);
  m_stackPointer--;
  Write(0x0100 + m_stackPointer, m_programCounter & 0x00FF);
  m_stackPointer--;

  SetFlag(B, 1);
  Write(0x0100 + m_stackPointer, m_status);
  m_stackPointer--;
  SetFlag(B, 0);

  m_programCounter = (uint16_t)Read(0xFFFE) | ((uint16_t)Read(0xFFFF) << 8);
  return 0;
}

// Instruction: Branch if Overflow Clear
// Function:    if(V == 0) pc = address
uint8_t Cpu::BVC()
{
  if (GetFlag(V) == 0)
  {
    m_cycles++;
    m_addrAbs = m_programCounter + m_addrRel;

    if ((m_addrAbs & 0xFF00) != (m_programCounter & 0xFF00))
      m_cycles++;

    m_programCounter = m_addrAbs;
  }
  return 0;
}

// Instruction: Branch if Overflow Set
// Function:    if(V == 1) pc = address
uint8_t Cpu::BVS()
{
  if (GetFlag(V) == 1)
  {
    m_cycles++;
    m_addrAbs = m_programCounter + m_addrRel;

    if ((m_addrAbs & 0xFF00) != (m_programCounter & 0xFF00))
      m_cycles++;

    m_programCounter = m_addrAbs;
  }
  return 0;
}

// Instruction: Clear Carry Flag
// Function:    C = 0
uint8_t Cpu::CLC()
{
  SetFlag(C, false);
  return 0;
}

// Instruction: Clear Decimal Flag
// Function:    D = 0
uint8_t Cpu::CLD()
{
  SetFlag(D, false);
  return 0;
}

// Instruction: Disable Interrupts / Clear Interrupt Flag
// Function:    I = 0
uint8_t Cpu::CLI()
{
  SetFlag(I, false);
  return 0;
}


// Instruction: Clear Overflow Flag
// Function:    V = 0
uint8_t Cpu::CLV()
{
  SetFlag(V, false);
  return 0;
}

// Instruction: Compare Accumulator
// Function:    C <- A >= M      Z <- (A - M) == 0
// Flags Out:   N, C, Z
uint8_t Cpu::CMP()
{
  Fetch();
  m_temp = (uint16_t)m_a - (uint16_t)m_fetched;
  SetFlag(C, m_a >= m_fetched);
  SetFlag(Z, (m_temp & 0x00FF) == 0x0000);
  SetFlag(N, m_temp & 0x0080);
  return 1;
}

// Instruction: Compare X Register
// Function:    C <- X >= M      Z <- (X - M) == 0
// Flags Out:   N, C, Z
uint8_t Cpu::CPX()
{
  Fetch();
  m_temp = (uint16_t)m_x - (uint16_t)m_fetched;
  SetFlag(C, m_x >= m_fetched);
  SetFlag(Z, (m_temp & 0x00FF) == 0x0000);
  SetFlag(N, m_temp & 0x0080);
  return 0;
}

// Instruction: Compare Y Register
// Function:    C <- Y >= M      Z <- (Y - M) == 0
// Flags Out:   N, C, Z
uint8_t Cpu::CPY()
{
  Fetch();
  m_temp = (uint16_t)m_y - (uint16_t)m_fetched;
  SetFlag(C, m_y >= m_fetched);
  SetFlag(Z, (m_temp & 0x00FF) == 0x0000);
  SetFlag(N, m_temp & 0x0080);
  return 0;
}

// Instruction: Decrement Value at Memory Location
// Function:    M = M - 1
// Flags Out:   N, Z
uint8_t Cpu::DEC()
{
  Fetch();
  m_temp = m_fetched - 1;
  Write(m_addrAbs, m_temp & 0x00FF);
  SetFlag(Z, (m_temp & 0x00FF) == 0x0000);
  SetFlag(N, m_temp & 0x0080);
  return 0;
}

// Instruction: Decrement X Register
// Function:    X = X - 1
// Flags Out:   N, Z
uint8_t Cpu::DEX()
{
  m_x--;
  SetFlag(Z, m_x == 0x00);
  SetFlag(N, m_x & 0x80);
  return 0;
}

// Instruction: Decrement Y Register
// Function:    Y = Y - 1
// Flags Out:   N, Z
uint8_t Cpu::DEY()
{
  m_y--;
  SetFlag(Z, m_y == 0x00);
  SetFlag(N, m_y & 0x80);
  return 0;
}

// Instruction: Bitwise Logic XOR
// Function:    A = A xor M
// Flags Out:   N, Z
uint8_t Cpu::EOR()
{
  Fetch();
  m_a = m_a ^ m_fetched;
  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & 0x80);
  return 1;
}

// Instruction: Increment Value at Memory Location
// Function:    M = M + 1
// Flags Out:   N, Z
uint8_t Cpu::INC()
{
  Fetch();
  m_temp = m_fetched + 1;
  Write(m_addrAbs, m_temp & 0x00FF);
  SetFlag(Z, (m_temp & 0x00FF) == 0x0000);
  SetFlag(N, m_temp & 0x0080);
  return 0;
}

// Instruction: Increment X Register
// Function:    X = X + 1
// Flags Out:   N, Z
uint8_t Cpu::INX()
{
  m_x++;
  SetFlag(Z, m_x == 0x00);
  SetFlag(N, m_x & 0x80);
  return 0;
}

// Instruction: Increment Y Register
// Function:    Y = Y + 1
// Flags Out:   N, Z
uint8_t Cpu::INY()
{
  m_y++;
  SetFlag(Z, m_y == 0x00);
  SetFlag(N, m_y & 0x80);
  return 0;
}

// Instruction: Jump To Location
// Function:    pc = address
uint8_t Cpu::JMP()
{
  m_programCounter = m_addrAbs;
  return 0;
}

// Instruction: Jump To Sub-Routine
// Function:    Push current pc to stack, pc = address
uint8_t Cpu::JSR()
{
  m_programCounter--;

  Write(0x0100 + m_stackPointer, (m_programCounter >> 8) & 0x00FF);
  m_stackPointer--;
  Write(0x0100 + m_stackPointer, m_programCounter & 0x00FF);
  m_stackPointer--;

  m_programCounter = m_addrAbs;
  return 0;
}

// Instruction: Load The Accumulator
// Function:    A = M
// Flags Out:   N, Z
uint8_t Cpu::LDA()
{
  Fetch();
  m_a = m_fetched;
  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & 0x80);
  return 1;
}

// Instruction: Load The X Register
// Function:    X = M
// Flags Out:   N, Z
uint8_t Cpu::LDX()
{
  Fetch();
  m_x = m_fetched;
  SetFlag(Z, m_x == 0x00);
  SetFlag(N, m_x & 0x80);
  return 1;
}

// Instruction: Load The Y Register
// Function:    Y = M
// Flags Out:   N, Z
uint8_t Cpu::LDY()
{
  Fetch();
  m_y = m_fetched;
  SetFlag(Z, m_y == 0x00);
  SetFlag(N, m_y & 0x80);
  return 1;
}

uint8_t Cpu::LSR()
{
  Fetch();
  SetFlag(C, m_fetched & 0x0001);
  m_temp = m_fetched >> 1;
  SetFlag(Z, (m_temp & 0x00FF) == 0x0000);
  SetFlag(N, m_temp & 0x0080);
  if (m_lookup[m_opcode].addrmode == &Cpu::IMP)
    m_a = m_temp & 0x00FF;
  else
    Write(m_addrAbs, m_temp & 0x00FF);

  return 0;
}

uint8_t Cpu::NOP()
{
  // Sadly not all NOPs are equal, Ive added a few here
  // based on https://wiki.nesdev.com/w/index.php/CPU_unofficial_opcodes
  // and will add more based on game compatibility, and ultimately
  // I'd like to cover all illegal opcodes too
  switch (m_opcode) {
  case 0x1C:
  case 0x3C:
  case 0x5C:
  case 0x7C:
  case 0xDC:
  case 0xFC:
    return 1;
    break;
  }
  return 0;
}

// Instruction: Bitwise Logic OR
// Function:    A = A | M
// Flags Out:   N, Z
uint8_t Cpu::ORA()
{
  Fetch();
  m_a = m_a | m_fetched;
  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & 0x80);
  return 1;
}

// Instruction: Push Accumulator to Stack
// Function:    A -> stack
uint8_t Cpu::PHA()
{
  Write(0x0100 + m_stackPointer, m_a);
  m_stackPointer--;
  return 0;
}

// Instruction: Push Status Register to Stack
// Function:    status -> stack
// Note:        Break flag is set to 1 before push
uint8_t Cpu::PHP()
{
  Write(0x0100 + m_stackPointer, m_status | B | U);
  SetFlag(B, 0);
  SetFlag(U, 0);
  m_stackPointer--;
  return 0;
}

// Instruction: Pop Accumulator off Stack
// Function:    A <- stack
// Flags Out:   N, Z
uint8_t Cpu::PLA()
{
  m_stackPointer++;
  m_a = Read(0x0100 + m_stackPointer);
  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & 0x80);
  return 0;
}

// Instruction: Pop Status Register off Stack
// Function:    Status <- stack
uint8_t Cpu::PLP()
{
  m_stackPointer++;
  m_status = Read(0x0100 + m_stackPointer);
  SetFlag(U, 1);
  return 0;
}

uint8_t Cpu::ROL()
{
  Fetch();
  m_temp = (uint16_t)(m_fetched << 1) | GetFlag(C);
  SetFlag(C, m_temp & 0xFF00);
  SetFlag(Z, (m_temp & 0x00FF) == 0x0000);
  SetFlag(N, m_temp & 0x0080);
  if (m_lookup[m_opcode].addrmode == &Cpu::IMP)
    m_a = m_temp & 0x00FF;
  else
    Write(m_addrAbs, m_temp & 0x00FF);
  return 0;
}

uint8_t Cpu::ROR()
{
  Fetch();
  m_temp = (uint16_t)(GetFlag(C) << 7) | (m_fetched >> 1);
  SetFlag(C, m_fetched & 0x01);
  SetFlag(Z, (m_temp & 0x00FF) == 0x00);
  SetFlag(N, m_temp & 0x0080);
  if (m_lookup[m_opcode].addrmode == &Cpu::IMP)
    m_a = m_temp & 0x00FF;
  else
    Write(m_addrAbs, m_temp & 0x00FF);
  return 0;
}

uint8_t Cpu::RTI()
{
  m_stackPointer++;
  m_status = Read(0x0100 + m_stackPointer);
  m_status &= ~B;
  m_status &= ~U;

  m_stackPointer++;
  m_programCounter = (uint16_t)Read(0x0100 + m_stackPointer);
  m_stackPointer++;
  m_programCounter |= (uint16_t)Read(0x0100 + m_stackPointer) << 8;
  return 0;
}

uint8_t Cpu::RTS()
{
  m_stackPointer++;
  m_programCounter = (uint16_t)Read(0x0100 + m_stackPointer);
  m_stackPointer++;
  m_programCounter |= (uint16_t)Read(0x0100 + m_stackPointer) << 8;

  m_programCounter++;
  return 0;
}

// Instruction: Set Carry Flag
// Function:    C = 1
uint8_t Cpu::SEC()
{
  SetFlag(C, true);
  return 0;
}

// Instruction: Set Decimal Flag
// Function:    D = 1
uint8_t Cpu::SED()
{
  SetFlag(D, true);
  return 0;
}

// Instruction: Set Interrupt Flag / Enable Interrupts
// Function:    I = 1
uint8_t Cpu::SEI()
{
  SetFlag(I, true);
  return 0;
}

// Instruction: Store Accumulator at Address
// Function:    M = A
uint8_t Cpu::STA()
{
  Write(m_addrAbs, m_a);
  return 0;
}

// Instruction: Store X Register at Address
// Function:    M = X
uint8_t Cpu::STX()
{
  Write(m_addrAbs, m_x);
  return 0;
}

// Instruction: Store Y Register at Address
// Function:    M = Y
uint8_t Cpu::STY()
{
  Write(m_addrAbs, m_y);
  return 0;
}

// Instruction: Transfer Accumulator to X Register
// Function:    X = A
// Flags Out:   N, Z
uint8_t Cpu::TAX()
{
  m_x = m_a;
  SetFlag(Z, m_x == 0x00);
  SetFlag(N, m_x & 0x80);
  return 0;
}

// Instruction: Transfer Accumulator to Y Register
// Function:    Y = A
// Flags Out:   N, Z
uint8_t Cpu::TAY()
{
  m_y = m_a;
  SetFlag(Z, m_y == 0x00);
  SetFlag(N, m_y & 0x80);
  return 0;
}

// Instruction: Transfer Stack Pointer to X Register
// Function:    X = stack pointer
// Flags Out:   N, Z
uint8_t Cpu::TSX()
{
  m_x = m_stackPointer;
  SetFlag(Z, m_x == 0x00);
  SetFlag(N, m_x & 0x80);
  return 0;
}

// Instruction: Transfer X Register to Accumulator
// Function:    A = X
// Flags Out:   N, Z
uint8_t Cpu::TXA()
{
  m_a = m_x;
  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & 0x80);
  return 0;
}

// Instruction: Transfer X Register to Stack Pointer
// Function:    stack pointer = X
uint8_t Cpu::TXS()
{
  m_stackPointer = m_x;
  return 0;
}

// Instruction: Transfer Y Register to Accumulator
// Function:    A = Y
// Flags Out:   N, Z
uint8_t Cpu::TYA()
{
  m_a = m_y;
  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & 0x80);
  return 0;
}

// This function captures illegal opcodes
uint8_t Cpu::XXX()
{
  return 0;
}
