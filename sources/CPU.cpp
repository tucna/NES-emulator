#include "Bus.h"
#include "CPU.h"

CPU::CPU()
{
  // Based on version taken from javidx9 - thanks!
  // {opcode, mnemonic, function pointer, address mode pointer, number clock cycles}
  m_lookup =
  {
    { 0x00, "BRK", &CPU::BRK, &CPU::IMM, 7 },{ 0x01, "ORA", &CPU::ORA, &CPU::IZX, 6 },{ 0x02, "KIL", &CPU::KIL, &CPU::IMP, 2 },{ 0x03, "SLO", &CPU::SLO, &CPU::IZX, 8 },{ 0x04, "NOP", &CPU::NOP, &CPU::ZP0, 3 },{ 0x05, "ORA", &CPU::ORA, &CPU::ZP0, 3 },{ 0x06, "ASL", &CPU::ASL, &CPU::ZP0, 5 },{ 0x07, "SLO", &CPU::SLO, &CPU::ZP0, 5 },{ 0x08, "PHP", &CPU::PHP, &CPU::IMP, 3 },{ 0x09, "ORA", &CPU::ORA, &CPU::IMM, 2 },{ 0x0A, "ASL", &CPU::ASL, &CPU::IMP, 2 },{ 0x0B, "???", &CPU::XXX, &CPU::IMP, 2 },{ 0x0C, "NOP", &CPU::NOP, &CPU::ABS, 4 },{ 0x0D, "ORA", &CPU::ORA, &CPU::ABS, 4 },{ 0x0E, "ASL", &CPU::ASL, &CPU::ABS, 6 },{ 0x0F, "SLO", &CPU::SLO, &CPU::ABS, 6 },
    { 0x10, "BPL", &CPU::BPL, &CPU::REL, 2 },{ 0x11, "ORA", &CPU::ORA, &CPU::IZY, 5 },{ 0x12, "KIL", &CPU::KIL, &CPU::IMP, 2 },{ 0x13, "SLO", &CPU::SLO, &CPU::IZY, 8 },{ 0x14, "NOP", &CPU::NOP, &CPU::ZPX, 4 },{ 0x15, "ORA", &CPU::ORA, &CPU::ZPX, 4 },{ 0x16, "ASL", &CPU::ASL, &CPU::ZPX, 6 },{ 0x17, "SLO", &CPU::SLO, &CPU::ZPX, 6 },{ 0x18, "CLC", &CPU::CLC, &CPU::IMP, 2 },{ 0x19, "ORA", &CPU::ORA, &CPU::ABY, 4 },{ 0x1A, "NOP", &CPU::NOP, &CPU::IMP, 2 },{ 0x1B, "SLO", &CPU::SLO, &CPU::ABY, 7 },{ 0x1C, "NOP", &CPU::NOP, &CPU::ABX, 4 },{ 0x1D, "ORA", &CPU::ORA, &CPU::ABX, 4 },{ 0x1E, "ASL", &CPU::ASL, &CPU::ABX, 7 },{ 0x1F, "SLO", &CPU::SLO, &CPU::ABX, 7 },
    { 0x20, "JSR", &CPU::JSR, &CPU::ABS, 6 },{ 0x21, "AND", &CPU::AND, &CPU::IZX, 6 },{ 0x22, "KIL", &CPU::KIL, &CPU::IMP, 2 },{ 0x23, "RLA", &CPU::RLA, &CPU::IZX, 8 },{ 0x24, "BIT", &CPU::BIT, &CPU::ZP0, 3 },{ 0x25, "AND", &CPU::AND, &CPU::ZP0, 3 },{ 0x26, "ROL", &CPU::ROL, &CPU::ZP0, 5 },{ 0x27, "RLA", &CPU::RLA, &CPU::ZP0, 5 },{ 0x28, "PLP", &CPU::PLP, &CPU::IMP, 4 },{ 0x29, "AND", &CPU::AND, &CPU::IMM, 2 },{ 0x2A, "ROL", &CPU::ROL, &CPU::IMP, 2 },{ 0x2B, "???", &CPU::XXX, &CPU::IMP, 2 },{ 0x2C, "BIT", &CPU::BIT, &CPU::ABS, 4 },{ 0x2D, "AND", &CPU::AND, &CPU::ABS, 4 },{ 0x2E, "ROL", &CPU::ROL, &CPU::ABS, 6 },{ 0x2F, "RLA", &CPU::RLA, &CPU::ABS, 6 },
    { 0x30, "BMI", &CPU::BMI, &CPU::REL, 2 },{ 0x31, "AND", &CPU::AND, &CPU::IZY, 5 },{ 0x32, "KIL", &CPU::KIL, &CPU::IMP, 2 },{ 0x33, "RLA", &CPU::RLA, &CPU::IZY, 8 },{ 0x34, "NOP", &CPU::NOP, &CPU::ZPX, 4 },{ 0x35, "AND", &CPU::AND, &CPU::ZPX, 4 },{ 0x36, "ROL", &CPU::ROL, &CPU::ZPX, 6 },{ 0x37, "RLA", &CPU::RLA, &CPU::ZPX, 6 },{ 0x38, "SEC", &CPU::SEC, &CPU::IMP, 2 },{ 0x39, "AND", &CPU::AND, &CPU::ABY, 4 },{ 0x3A, "NOP", &CPU::NOP, &CPU::IMP, 2 },{ 0x3B, "RLA", &CPU::RLA, &CPU::ABY, 7 },{ 0x3C, "NOP", &CPU::NOP, &CPU::ABX, 4 },{ 0x3D, "AND", &CPU::AND, &CPU::ABX, 4 },{ 0x3E, "ROL", &CPU::ROL, &CPU::ABX, 7 },{ 0x3F, "RLA", &CPU::RLA, &CPU::ABX, 7 },
    { 0x40, "RTI", &CPU::RTI, &CPU::IMP, 6 },{ 0x41, "EOR", &CPU::EOR, &CPU::IZX, 6 },{ 0x42, "KIL", &CPU::KIL, &CPU::IMP, 2 },{ 0x43, "SRE", &CPU::SRE, &CPU::IZX, 8 },{ 0x44, "NOP", &CPU::NOP, &CPU::ZP0, 3 },{ 0x45, "EOR", &CPU::EOR, &CPU::ZP0, 3 },{ 0x46, "LSR", &CPU::LSR, &CPU::ZP0, 5 },{ 0x47, "SRE", &CPU::SRE, &CPU::ZP0, 5 },{ 0x48, "PHA", &CPU::PHA, &CPU::IMP, 3 },{ 0x49, "EOR", &CPU::EOR, &CPU::IMM, 2 },{ 0x4A, "LSR", &CPU::LSR, &CPU::IMP, 2 },{ 0x4B, "???", &CPU::XXX, &CPU::IMP, 2 },{ 0x4C, "JMP", &CPU::JMP, &CPU::ABS, 3 },{ 0x4D, "EOR", &CPU::EOR, &CPU::ABS, 4 },{ 0x4E, "LSR", &CPU::LSR, &CPU::ABS, 6 },{ 0x4F, "SRE", &CPU::SRE, &CPU::ABS, 6 },
    { 0x50, "BVC", &CPU::BVC, &CPU::REL, 2 },{ 0x51, "EOR", &CPU::EOR, &CPU::IZY, 5 },{ 0x52, "KIL", &CPU::KIL, &CPU::IMP, 2 },{ 0x53, "SRE", &CPU::SRE, &CPU::IZY, 8 },{ 0x54, "NOP", &CPU::NOP, &CPU::ZPX, 4 },{ 0x55, "EOR", &CPU::EOR, &CPU::ZPX, 4 },{ 0x56, "LSR", &CPU::LSR, &CPU::ZPX, 6 },{ 0x57, "SRE", &CPU::SRE, &CPU::ZPX, 6 },{ 0x58, "CLI", &CPU::CLI, &CPU::IMP, 2 },{ 0x59, "EOR", &CPU::EOR, &CPU::ABY, 4 },{ 0x5A, "NOP", &CPU::NOP, &CPU::IMP, 2 },{ 0x5B, "SRE", &CPU::SRE, &CPU::ABY, 7 },{ 0x5C, "NOP", &CPU::NOP, &CPU::ABX, 4 },{ 0x5D, "EOR", &CPU::EOR, &CPU::ABX, 4 },{ 0x5E, "LSR", &CPU::LSR, &CPU::ABX, 7 },{ 0x5F, "SRE", &CPU::SRE, &CPU::ABX, 7 },
    { 0x60, "RTS", &CPU::RTS, &CPU::IMP, 6 },{ 0x61, "ADC", &CPU::ADC, &CPU::IZX, 6 },{ 0x62, "KIL", &CPU::KIL, &CPU::IMP, 2 },{ 0x63, "RRA", &CPU::RRA, &CPU::IZX, 8 },{ 0x64, "NOP", &CPU::NOP, &CPU::ZP0, 3 },{ 0x65, "ADC", &CPU::ADC, &CPU::ZP0, 3 },{ 0x66, "ROR", &CPU::ROR, &CPU::ZP0, 5 },{ 0x67, "RRA", &CPU::RRA, &CPU::ZP0, 5 },{ 0x68, "PLA", &CPU::PLA, &CPU::IMP, 4 },{ 0x69, "ADC", &CPU::ADC, &CPU::IMM, 2 },{ 0x6A, "ROR", &CPU::ROR, &CPU::IMP, 2 },{ 0x6B, "???", &CPU::XXX, &CPU::IMP, 2 },{ 0x6C, "JMP", &CPU::JMP, &CPU::IND, 5 },{ 0x6D, "ADC", &CPU::ADC, &CPU::ABS, 4 },{ 0x6E, "ROR", &CPU::ROR, &CPU::ABS, 6 },{ 0x6F, "RRA", &CPU::RRA, &CPU::ABS, 6 },
    { 0x70, "BVS", &CPU::BVS, &CPU::REL, 2 },{ 0x71, "ADC", &CPU::ADC, &CPU::IZY, 5 },{ 0x72, "KIL", &CPU::KIL, &CPU::IMP, 2 },{ 0x73, "RRA", &CPU::RRA, &CPU::IZY, 8 },{ 0x74, "NOP", &CPU::NOP, &CPU::ZPX, 4 },{ 0x75, "ADC", &CPU::ADC, &CPU::ZPX, 4 },{ 0x76, "ROR", &CPU::ROR, &CPU::ZPX, 6 },{ 0x77, "RRA", &CPU::RRA, &CPU::ZPX, 6 },{ 0x78, "SEI", &CPU::SEI, &CPU::IMP, 2 },{ 0x79, "ADC", &CPU::ADC, &CPU::ABY, 4 },{ 0x7A, "NOP", &CPU::NOP, &CPU::IMP, 2 },{ 0x7B, "RRA", &CPU::RRA, &CPU::ABY, 7 },{ 0x7C, "NOP", &CPU::NOP, &CPU::ABX, 4 },{ 0x7D, "ADC", &CPU::ADC, &CPU::ABX, 4 },{ 0x7E, "ROR", &CPU::ROR, &CPU::ABX, 7 },{ 0x7F, "RRA", &CPU::RRA, &CPU::ABX, 7 },
    { 0x80, "NOP", &CPU::NOP, &CPU::IMM, 2 },{ 0x81, "STA", &CPU::STA, &CPU::IZX, 6 },{ 0x82, "NOP", &CPU::NOP, &CPU::IMM, 2 },{ 0x83, "SAX", &CPU::SAX, &CPU::IZX, 6 },{ 0x84, "STY", &CPU::STY, &CPU::ZP0, 3 },{ 0x85, "STA", &CPU::STA, &CPU::ZP0, 3 },{ 0x86, "STX", &CPU::STX, &CPU::ZP0, 3 },{ 0x87, "SAX", &CPU::SAX, &CPU::ZP0, 3 },{ 0x88, "DEY", &CPU::DEY, &CPU::IMP, 2 },{ 0x89, "NOP", &CPU::NOP, &CPU::IMM, 2 },{ 0x8A, "TXA", &CPU::TXA, &CPU::IMP, 2 },{ 0x8B, "???", &CPU::XXX, &CPU::IMP, 2 },{ 0x8C, "STY", &CPU::STY, &CPU::ABS, 4 },{ 0x8D, "STA", &CPU::STA, &CPU::ABS, 4 },{ 0x8E, "STX", &CPU::STX, &CPU::ABS, 4 },{ 0x8F, "SAX", &CPU::SAX, &CPU::ABS, 4 },
    { 0x90, "BCC", &CPU::BCC, &CPU::REL, 2 },{ 0x91, "STA", &CPU::STA, &CPU::IZY, 6 },{ 0x92, "KIL", &CPU::KIL, &CPU::IMP, 2 },{ 0x93, "???", &CPU::XXX, &CPU::IMP, 6 },{ 0x94, "STY", &CPU::STY, &CPU::ZPX, 4 },{ 0x95, "STA", &CPU::STA, &CPU::ZPX, 4 },{ 0x96, "STX", &CPU::STX, &CPU::ZPY, 4 },{ 0x97, "SAX", &CPU::SAX, &CPU::ZPY, 4 },{ 0x98, "TYA", &CPU::TYA, &CPU::IMP, 2 },{ 0x99, "STA", &CPU::STA, &CPU::ABY, 5 },{ 0x9A, "TXS", &CPU::TXS, &CPU::IMP, 2 },{ 0x9B, "???", &CPU::XXX, &CPU::IMP, 5 },{ 0x9C, "???", &CPU::XXX, &CPU::IMP, 5 },{ 0x9D, "STA", &CPU::STA, &CPU::ABX, 5 },{ 0x9E, "???", &CPU::XXX, &CPU::IMP, 5 },{ 0x9F, "???", &CPU::XXX, &CPU::IMP, 5 },
    { 0xA0, "LDY", &CPU::LDY, &CPU::IMM, 2 },{ 0xA1, "LDA", &CPU::LDA, &CPU::IZX, 6 },{ 0xA2, "LDX", &CPU::LDX, &CPU::IMM, 2 },{ 0xA3, "LAX", &CPU::LAX, &CPU::IZX, 6 },{ 0xA4, "LDY", &CPU::LDY, &CPU::ZP0, 3 },{ 0xA5, "LDA", &CPU::LDA, &CPU::ZP0, 3 },{ 0xA6, "LDX", &CPU::LDX, &CPU::ZP0, 3 },{ 0xA7, "LAX", &CPU::LAX, &CPU::ZP0, 3 },{ 0xA8, "TAY", &CPU::TAY, &CPU::IMP, 2 },{ 0xA9, "LDA", &CPU::LDA, &CPU::IMM, 2 },{ 0xAA, "TAX", &CPU::TAX, &CPU::IMP, 2 },{ 0xAB, "???", &CPU::XXX, &CPU::IMP, 2 },{ 0xAC, "LDY", &CPU::LDY, &CPU::ABS, 4 },{ 0xAD, "LDA", &CPU::LDA, &CPU::ABS, 4 },{ 0xAE, "LDX", &CPU::LDX, &CPU::ABS, 4 },{ 0xAF, "LAX", &CPU::LAX, &CPU::ABS, 4 },
    { 0xB0, "BCS", &CPU::BCS, &CPU::REL, 2 },{ 0xB1, "LDA", &CPU::LDA, &CPU::IZY, 5 },{ 0xB2, "KIL", &CPU::KIL, &CPU::IMP, 2 },{ 0xB3, "LAX", &CPU::LAX, &CPU::IZY, 5 },{ 0xB4, "LDY", &CPU::LDY, &CPU::ZPX, 4 },{ 0xB5, "LDA", &CPU::LDA, &CPU::ZPX, 4 },{ 0xB6, "LDX", &CPU::LDX, &CPU::ZPY, 4 },{ 0xB7, "LAX", &CPU::LAX, &CPU::ZPY, 4 },{ 0xB8, "CLV", &CPU::CLV, &CPU::IMP, 2 },{ 0xB9, "LDA", &CPU::LDA, &CPU::ABY, 4 },{ 0xBA, "TSX", &CPU::TSX, &CPU::IMP, 2 },{ 0xBB, "???", &CPU::XXX, &CPU::IMP, 4 },{ 0xBC, "LDY", &CPU::LDY, &CPU::ABX, 4 },{ 0xBD, "LDA", &CPU::LDA, &CPU::ABX, 4 },{ 0xBE, "LDX", &CPU::LDX, &CPU::ABY, 4 },{ 0xBF, "LAX", &CPU::LAX, &CPU::ABY, 4 },
    { 0xC0, "CPY", &CPU::CPY, &CPU::IMM, 2 },{ 0xC1, "CMP", &CPU::CMP, &CPU::IZX, 6 },{ 0xC2, "NOP", &CPU::NOP, &CPU::IMM, 2 },{ 0xC3, "DCP", &CPU::DCP, &CPU::IZX, 8 },{ 0xC4, "CPY", &CPU::CPY, &CPU::ZP0, 3 },{ 0xC5, "CMP", &CPU::CMP, &CPU::ZP0, 3 },{ 0xC6, "DEC", &CPU::DEC, &CPU::ZP0, 5 },{ 0xC7, "DCP", &CPU::DCP, &CPU::ZP0, 5 },{ 0xC8, "INY", &CPU::INY, &CPU::IMP, 2 },{ 0xC9, "CMP", &CPU::CMP, &CPU::IMM, 2 },{ 0xCA, "DEX", &CPU::DEX, &CPU::IMP, 2 },{ 0xCB, "???", &CPU::XXX, &CPU::IMP, 2 },{ 0xCC, "CPY", &CPU::CPY, &CPU::ABS, 4 },{ 0xCD, "CMP", &CPU::CMP, &CPU::ABS, 4 },{ 0xCE, "DEC", &CPU::DEC, &CPU::ABS, 6 },{ 0xCF, "DCP", &CPU::DCP, &CPU::ABS, 6 },
    { 0xD0, "BNE", &CPU::BNE, &CPU::REL, 2 },{ 0xD1, "CMP", &CPU::CMP, &CPU::IZY, 5 },{ 0xD2, "KIL", &CPU::KIL, &CPU::IMP, 2 },{ 0xD3, "DCP", &CPU::DCP, &CPU::IZY, 8 },{ 0xD4, "NOP", &CPU::NOP, &CPU::ZPX, 4 },{ 0xD5, "CMP", &CPU::CMP, &CPU::ZPX, 4 },{ 0xD6, "DEC", &CPU::DEC, &CPU::ZPX, 6 },{ 0xD7, "DCP", &CPU::DCP, &CPU::ZPX, 6 },{ 0xD8, "CLD", &CPU::CLD, &CPU::IMP, 2 },{ 0xD9, "CMP", &CPU::CMP, &CPU::ABY, 4 },{ 0xDA, "NOP", &CPU::NOP, &CPU::IMP, 2 },{ 0xDB, "DCP", &CPU::DCP, &CPU::ABY, 7 },{ 0xDC, "NOP", &CPU::NOP, &CPU::ABX, 4 },{ 0xDD, "CMP", &CPU::CMP, &CPU::ABX, 4 },{ 0xDE, "DEC", &CPU::DEC, &CPU::ABX, 7 },{ 0xDF, "DCP", &CPU::DCP, &CPU::ABX, 7 },
    { 0xE0, "CPX", &CPU::CPX, &CPU::IMM, 2 },{ 0xE1, "SBC", &CPU::SBC, &CPU::IZX, 6 },{ 0xE2, "NOP", &CPU::NOP, &CPU::IMM, 2 },{ 0xE3, "ISC", &CPU::ISC, &CPU::IZX, 8 },{ 0xE4, "CPX", &CPU::CPX, &CPU::ZP0, 3 },{ 0xE5, "SBC", &CPU::SBC, &CPU::ZP0, 3 },{ 0xE6, "INC", &CPU::INC, &CPU::ZP0, 5 },{ 0xE7, "ISC", &CPU::ISC, &CPU::ZP0, 5 },{ 0xE8, "INX", &CPU::INX, &CPU::IMP, 2 },{ 0xE9, "SBC", &CPU::SBC, &CPU::IMM, 2 },{ 0xEA, "NOP", &CPU::NOP, &CPU::IMP, 2 },{ 0xEB, "SBC", &CPU::SBC, &CPU::IMM, 2 },{ 0xEC, "CPX", &CPU::CPX, &CPU::ABS, 4 },{ 0xED, "SBC", &CPU::SBC, &CPU::ABS, 4 },{ 0xEE, "INC", &CPU::INC, &CPU::ABS, 6 },{ 0xEF, "ISC", &CPU::ISC, &CPU::ABS, 6 },
    { 0xF0, "BEQ", &CPU::BEQ, &CPU::REL, 2 },{ 0xF1, "SBC", &CPU::SBC, &CPU::IZY, 5 },{ 0xF2, "KIL", &CPU::KIL, &CPU::IMP, 2 },{ 0xF3, "ISC", &CPU::ISC, &CPU::IZY, 8 },{ 0xF4, "NOP", &CPU::NOP, &CPU::ZPX, 4 },{ 0xF5, "SBC", &CPU::SBC, &CPU::ZPX, 4 },{ 0xF6, "INC", &CPU::INC, &CPU::ZPX, 6 },{ 0xF7, "ISC", &CPU::ISC, &CPU::ZPX, 6 },{ 0xF8, "SED", &CPU::SED, &CPU::IMP, 2 },{ 0xF9, "SBC", &CPU::SBC, &CPU::ABY, 4 },{ 0xFA, "NOP", &CPU::NOP, &CPU::IMP, 2 },{ 0xFB, "ISC", &CPU::ISC, &CPU::ABY, 7 },{ 0xFC, "NOP", &CPU::NOP, &CPU::ABX, 4 },{ 0xFD, "SBC", &CPU::SBC, &CPU::ABX, 4 },{ 0xFE, "INC", &CPU::INC, &CPU::ABX, 7 },{ 0xFF, "ISC", &CPU::ISC, &CPU::ABX, 7 },
  };
}

void CPU::PowerUp()
{
  // Get address to set program counter to
  m_addrAbs = 0xFFFC;
  uint16_t lo = Read(m_addrAbs + 0);
  uint16_t hi = Read(m_addrAbs + 1);

  // Set it
  m_resetVectorPC = (hi << 8) | lo;

  // Set registers
  m_a = 0;
  m_x = 0;
  m_y = 0;
  m_stackPointer = 0xFD;
  m_status = 0x34;
  m_programCounter = m_resetVectorPC;

  // Clear internal helper variables
  m_addrRel = 0x0000;
  m_addrAbs = 0x0000;
  m_fetched = 0x00;

  // TODO
  /*
  $4017 = $00(frame irq enabled)
  $4015 = $00(all channels disabled)
  $4000 - $400F = $00
  $4010 - $4013 = $00[4]
  APU
  */
}

void CPU::Reset()
{
  // Registers
  m_status |= I;
  m_stackPointer -= 0x03;
  m_programCounter = m_resetVectorPC;

  // Clear internal helper variables
  m_addrRel = 0x0000;
  m_addrAbs = 0x0000;
  m_fetched = 0x00;

  // Reset takes time
  m_cycles = 8;
}

void CPU::IRQ()
{
  // If interrupts are allowed
  if (GetFlag(I) == 0)
  {
    // Push the program counter to the stack. It's 16-bits dont
    // forget so that takes two pushes
    Write(0x0100 + m_stackPointer, (m_programCounter >> 8) & 0x00FF);
    m_stackPointer--;
    Write(0x0100 + m_stackPointer, m_programCounter & 0x00FF);
    m_stackPointer--;

    // Then Push the status register to the stack
    SetFlag(B, 0);
    SetFlag(U, 1);
    SetFlag(I, 1);
    Write(0x0100 + m_stackPointer, m_status);
    m_stackPointer--;

    // Read new program counter location from fixed address
    m_addrAbs = 0xFFFE;
    uint16_t lo = Read(m_addrAbs + 0);
    uint16_t hi = Read(m_addrAbs + 1);
    m_programCounter = (hi << 8) | lo;

    // IRQs take time
    m_cycles = 7;
  }
}

void CPU::Clock()
{
  if (m_cycles == 0)
  {
    m_opcode = Read(m_programCounter);

    // Always set the unused status flag bit to 1
    SetFlag(U, true);

    // Increment program counter, we read the opcode byte
    m_programCounter++;

    // Get Starting number of cycles
    m_cycles = m_lookup[m_opcode].cycles;

    // Perform fetch of intermmediate data using the
    // required addressing mode
    uint8_t additional_cycle1 = (this->*m_lookup[m_opcode].addrmode)();

    // Perform operation
    uint8_t additional_cycle2 = (this->*m_lookup[m_opcode].operate)();

    // The addressmode and opcode may have altered the number
    // of cycles this instruction requires before its completed
    m_cycles += (additional_cycle1 & additional_cycle2);

    // Always set the unused status flag bit to 1
    SetFlag(U, true);
  }

  // Increment global clock count - This is actually unused unless logging is enabled
  // but I've kept it in because its a handy watch variable for debugging
  m_clockCount++;

  // Decrement the number of cycles remaining for this instruction
  m_cycles--;
}

void CPU::NMI()
{
  Write(0x0100 + m_stackPointer, (m_programCounter >> 8) & LO_BYTE);
  m_stackPointer--;
  Write(0x0100 + m_stackPointer, m_programCounter & LO_BYTE);
  m_stackPointer--;

  SetFlag(B, 0);
  SetFlag(U, 1);
  SetFlag(I, 1);
  Write(0x0100 + m_stackPointer, m_status);
  m_stackPointer--;

  m_addrAbs = 0xFFFA;
  uint16_t lo = Read(m_addrAbs + 0);
  uint16_t hi = Read(m_addrAbs + 1);
  m_programCounter = (hi << 8) | lo;

  m_cycles = 8;
}

std::map<uint16_t, std::string> CPU::Disassemble()
{
  uint8_t value = 0x00;
  uint8_t lo = 0x00;
  uint8_t hi = 0x00;
  uint16_t lineAddr = 0;
  uint32_t addr = m_resetVectorPC;

  std::map<uint16_t, std::string> mapLines;

  auto hex = [](uint32_t n, uint8_t d)
  {
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
      s[i] = "0123456789ABCDEF"[n & 0xF];

    return s;
  };

  while (addr <= 0xFFFF)
  {
    lineAddr = addr;

    // Prefix line with instruction address
    std::string sInst = "$" + hex(addr, 4) + ": ";

    // Read instruction, and get its readable name
    uint8_t opcode = m_bus->Read(addr, true); addr++;

    sInst += m_lookup[opcode].name + " ";

    if (m_lookup[opcode].addrmode == &CPU::IMP)
    {
      sInst += " {IMP}";
    }
    else if (m_lookup[opcode].addrmode == &CPU::IMM)
    {
      value = m_bus->Read(addr, true); addr++;
      sInst += "#$" + hex(value, 2) + " {IMM}";
    }
    else if (m_lookup[opcode].addrmode == &CPU::ZP0)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = 0x00;
      sInst += "$" + hex(lo, 2) + " {ZP0}";
    }
    else if (m_lookup[opcode].addrmode == &CPU::ZPX)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = 0x00;
      sInst += "$" + hex(lo, 2) + ", X {ZPX}";
    }
    else if (m_lookup[opcode].addrmode == &CPU::ZPY)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = 0x00;
      sInst += "$" + hex(lo, 2) + ", Y {ZPY}";
    }
    else if (m_lookup[opcode].addrmode == &CPU::IZX)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = 0x00;
      sInst += "($" + hex(lo, 2) + ", X) {IZX}";
    }
    else if (m_lookup[opcode].addrmode == &CPU::IZY)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = 0x00;
      sInst += "($" + hex(lo, 2) + "), Y {IZY}";
    }
    else if (m_lookup[opcode].addrmode == &CPU::ABS)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = m_bus->Read(addr, true); addr++;
      sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + " {ABS}";
    }
    else if (m_lookup[opcode].addrmode == &CPU::ABX)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = m_bus->Read(addr, true); addr++;
      sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", X {ABX}";
    }
    else if (m_lookup[opcode].addrmode == &CPU::ABY)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = m_bus->Read(addr, true); addr++;
      sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", Y {ABY}";
    }
    else if (m_lookup[opcode].addrmode == &CPU::IND)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = m_bus->Read(addr, true); addr++;
      sInst += "($" + hex((uint16_t)(hi << 8) | lo, 4) + ") {IND}";
    }
    else if (m_lookup[opcode].addrmode == &CPU::REL)
    {
      value = m_bus->Read(addr, true); addr++;
      sInst += "$" + hex(value, 2) + " [$" + hex(addr + (int8_t)value, 4) + "] {REL}"; // Offset (value) can range from -128 to +127 thus the (int8_t) cast.
    }

    mapLines[lineAddr] = sInst;
  }

  return mapLines;
}

// Address Mode: Implied
// There is no additional data required for this instruction. The instruction
// does something very simple like like sets a status bit. However, we will
// target the accumulator, for instructions like PHA
uint8_t CPU::IMP()
{
  m_fetched = m_a;

  return 0;
}

// Address Mode: Immediate
// The instruction expects the next byte to be used as a value, so we'll prep
// the read address to point to the next byte
uint8_t CPU::IMM()
{
  m_addrAbs = m_programCounter++;

  return 0;
}

// Address Mode: Zero Page
// To save program bytes, zero page addressing allows you to absolutely address
// a location in first 0xFF bytes of address range. Clearly this only requires
// one byte instead of the usual two.
uint8_t CPU::ZP0()
{
  m_addrAbs = Read(m_programCounter);
  m_programCounter++;
  m_addrAbs &= LO_BYTE;

  return 0;
}

// Address Mode: Zero Page with X Offset
// Fundamentally the same as Zero Page addressing, but the contents of the X Register
// is added to the supplied single byte address. This is useful for iterating through
// ranges within the first page.
uint8_t CPU::ZPX()
{
  m_addrAbs = (Read(m_programCounter) + m_x);
  m_programCounter++;
  m_addrAbs &= LO_BYTE;

  return 0;
}

// Address Mode: Zero Page with Y Offset
// Same as above but uses Y Register for offset
uint8_t CPU::ZPY()
{
  m_addrAbs = (Read(m_programCounter) + m_y);
  m_programCounter++;
  m_addrAbs &= LO_BYTE;

  return 0;
}

// Address Mode: Relative
// This address mode is exclusive to branch instructions. The address
// must reside within -128 to +127 of the branch instruction, i.e.
// you cant directly branch to any address in the addressable range.
uint8_t CPU::REL()
{
  m_addrRel = Read(m_programCounter);
  m_programCounter++;

  if (m_addrRel & NEGATIVE_MASK)
    m_addrRel |= HI_BYTE;

  return 0;
}

// Address Mode: Absolute
// A full 16-bit address is loaded and used
uint8_t CPU::ABS()
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
uint8_t CPU::ABX()
{
  uint16_t lo = Read(m_programCounter);
  m_programCounter++;
  uint16_t hi = Read(m_programCounter);
  m_programCounter++;

  m_addrAbs = (hi << 8) | lo;
  m_addrAbs += m_x;

  if ((m_addrAbs & HI_BYTE) != (hi << 8))
    return 1;
  else
    return 0;
}

// Address Mode: Absolute with Y Offset
// Fundamentally the same as absolute addressing, but the contents of the Y Register
// is added to the supplied two byte address. If the resulting address changes
// the page, an additional clock cycle is required
uint8_t CPU::ABY()
{
  uint16_t lo = Read(m_programCounter);
  m_programCounter++;
  uint16_t hi = Read(m_programCounter);
  m_programCounter++;

  m_addrAbs = (hi << 8) | lo;
  m_addrAbs += m_y;

  if ((m_addrAbs & HI_BYTE) != (hi << 8))
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
uint8_t CPU::IND()
{
  uint16_t ptr_lo = Read(m_programCounter);
  m_programCounter++;
  uint16_t ptr_hi = Read(m_programCounter);
  m_programCounter++;

  uint16_t ptr = (ptr_hi << 8) | ptr_lo;

  if (ptr_lo == LO_BYTE) // Simulate page boundary hardware bug
    m_addrAbs = (Read(ptr & HI_BYTE) << 8) | Read(ptr + 0);
  else // Behave normally
    m_addrAbs = (Read(ptr + 1) << 8) | Read(ptr + 0);

  return 0;
}

// Address Mode: Indirect X
// The supplied 8-bit address is offset by X Register to index
// a location in page 0x00. The actual 16-bit address is read
// from this location
uint8_t CPU::IZX()
{
  uint16_t t = Read(m_programCounter);
  m_programCounter++;

  uint16_t lo = Read((uint16_t)(t + (uint16_t)m_x) & LO_BYTE);
  uint16_t hi = Read((uint16_t)(t + (uint16_t)m_x + 1) & LO_BYTE);

  m_addrAbs = (hi << 8) | lo;

  return 0;
}

// Address Mode: Indirect Y
// The supplied 8-bit address indexes a location in page 0x00. From
// here the actual 16-bit address is read, and the contents of
// Y Register is added to it to offset it. If the offset causes a
// change in page then an additional clock cycle is required.
uint8_t CPU::IZY()
{
  uint16_t t = Read(m_programCounter);
  m_programCounter++;

  uint16_t lo = Read(t & LO_BYTE);
  uint16_t hi = Read((t + 1) & LO_BYTE);

  m_addrAbs = (hi << 8) | lo;
  m_addrAbs += m_y;

  if ((m_addrAbs & HI_BYTE) != (hi << 8))
    return 1;
  else
    return 0;
}

void CPU::Write(uint16_t addr, uint8_t data)
{
  m_bus->Write(addr, data);
}

uint8_t CPU::Read(uint16_t addr)
{
  return m_bus->Read(addr, false);
}

uint8_t CPU::Fetch()
{
  if (!(m_lookup[m_opcode].addrmode == &CPU::IMP))
    m_fetched = Read(m_addrAbs);

  return m_fetched;
}

uint8_t CPU::GetFlag(Flags flag)
{
  return ((m_status & flag) > 0) ? 1 : 0;
}

void CPU::SetFlag(Flags flag, bool setClear)
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
uint8_t CPU::ADC()
{
  // Grab the data that we are adding to the accumulator
  Fetch();

  // Add is performed in 16-bit domain for emulation to capture any
  // carry bit, which will exist in bit 8 of the 16-bit word
  m_temp = (uint16_t)m_a + (uint16_t)m_fetched + (uint16_t)GetFlag(C);

  // The carry flag out exists in the high byte bit 0
  SetFlag(C, m_temp > 255);

  // The Zero flag is set if the result is 0
  SetFlag(Z, (m_temp & LO_BYTE) == 0);

  // The signed Overflow flag is set based on all that up there! :D
  SetFlag(V, (~((uint16_t)m_a ^ (uint16_t)m_fetched) & ((uint16_t)m_a ^ (uint16_t)m_temp)) & NEGATIVE_MASK);

  // The negative flag is set to the most significant bit of the result
  SetFlag(N, m_temp & NEGATIVE_MASK);

  // Load the result into the accumulator (it's 8-bit dont forget!)
  m_a = m_temp & LO_BYTE;

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
uint8_t CPU::SBC()
{
  Fetch();

  // Operating in 16-bit domain to capture carry out

  // We can invert the bottom 8 bits with bitwise xor
  uint16_t value = ((uint16_t)m_fetched) ^ LO_BYTE;

  // Notice this is exactly the same as addition from here!
  m_temp = (uint16_t)m_a + value + (uint16_t)GetFlag(C);

  SetFlag(C, m_temp & HI_BYTE);
  SetFlag(Z, ((m_temp & LO_BYTE) == 0));
  SetFlag(V, (m_temp ^ (uint16_t)m_a) & (m_temp ^ value) & NEGATIVE_MASK);
  SetFlag(N, m_temp & NEGATIVE_MASK);

  m_a = m_temp & LO_BYTE;

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

// AND accumulator and value
uint8_t CPU::AND()
{
  Fetch();
  m_a &= m_fetched;

  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & NEGATIVE_MASK);

  return 1;
}

// Arithmetic Shift Left
uint8_t CPU::ASL()
{
  Fetch();
  m_temp = (uint16_t)m_fetched << 1;

  if (m_lookup[m_opcode].addrmode == &CPU::IMP)
    m_a = m_temp & LO_BYTE;
  else
    Write(m_addrAbs, m_temp & LO_BYTE);

  SetFlag(C, (m_temp & HI_BYTE) > 0);
  SetFlag(Z, (m_temp & LO_BYTE) == 0x00);
  SetFlag(N, m_temp & NEGATIVE_MASK);

  return 0;
}

// Branch on Carry Clear
uint8_t CPU::BCC()
{
  if (GetFlag(C) == 0)
  {
    m_cycles++;
    m_addrAbs = m_programCounter + m_addrRel;

    if ((m_addrAbs & HI_BYTE) != (m_programCounter & HI_BYTE))
      m_cycles++;

    m_programCounter = m_addrAbs;
  }

  return 0;
}

// Branch on Carry Set
uint8_t CPU::BCS()
{
  if (GetFlag(C) == 1)
  {
    m_cycles++;
    m_addrAbs = m_programCounter + m_addrRel;

    if ((m_addrAbs & HI_BYTE) != (m_programCounter & HI_BYTE))
      m_cycles++;

    m_programCounter = m_addrAbs;
  }

  return 0;
}

// Branch on EQual
uint8_t CPU::BEQ()
{
  if (GetFlag(Z) == 1)
  {
    m_cycles++;
    m_addrAbs = m_programCounter + m_addrRel;

    if ((m_addrAbs & HI_BYTE) != (m_programCounter & HI_BYTE))
      m_cycles++;

    m_programCounter = m_addrAbs;
  }

  return 0;
}

// test BITs
uint8_t CPU::BIT()
{
  Fetch();
  m_temp = m_a & m_fetched;

  SetFlag(Z, (m_temp & LO_BYTE) == 0x00);
  SetFlag(N, m_fetched & (1 << 7));
  SetFlag(V, m_fetched & (1 << 6));

  return 0;
}

// Branch on MInus
uint8_t CPU::BMI()
{
  if (GetFlag(N) == 1)
  {
    m_cycles++;
    m_addrAbs = m_programCounter + m_addrRel;

    if ((m_addrAbs & HI_BYTE) != (m_programCounter & HI_BYTE))
      m_cycles++;

    m_programCounter = m_addrAbs;
  }

  return 0;
}

// Branch on Not Equal
uint8_t CPU::BNE()
{
  if (GetFlag(Z) == 0)
  {
    m_cycles++;
    m_addrAbs = m_programCounter + m_addrRel;

    if ((m_addrAbs & HI_BYTE) != (m_programCounter & HI_BYTE))
      m_cycles++;

    m_programCounter = m_addrAbs;
  }

  return 0;
}

// Branch on PLus
uint8_t CPU::BPL()
{
  if (GetFlag(N) == 0)
  {
    m_cycles++;
    m_addrAbs = m_programCounter + m_addrRel;

    if ((m_addrAbs & HI_BYTE) != (m_programCounter & HI_BYTE))
      m_cycles++;

    m_programCounter = m_addrAbs;
  }

  return 0;
}

// BReaK
uint8_t CPU::BRK()
{
  m_programCounter++;

  Write(0x0100 + m_stackPointer, (m_programCounter >> 8) & LO_BYTE);
  m_stackPointer--;
  Write(0x0100 + m_stackPointer, m_programCounter & LO_BYTE);
  m_stackPointer--;

  SetFlag(I, 1);
  SetFlag(B, 1);

  Write(0x0100 + m_stackPointer, m_status);
  m_stackPointer--;

  SetFlag(B, 0);

  m_programCounter = (uint16_t)Read(0xFFFE) | ((uint16_t)Read(0xFFFF) << 8);

  return 0;
}

// Branch on oVerflow Clear
uint8_t CPU::BVC()
{
  if (GetFlag(V) == 0)
  {
    m_cycles++;
    m_addrAbs = m_programCounter + m_addrRel;

    if ((m_addrAbs & HI_BYTE) != (m_programCounter & HI_BYTE))
      m_cycles++;

    m_programCounter = m_addrAbs;
  }

  return 0;
}

// Branch on oVerflow Set
uint8_t CPU::BVS()
{
  if (GetFlag(V) == 1)
  {
    m_cycles++;
    m_addrAbs = m_programCounter + m_addrRel;

    if ((m_addrAbs & HI_BYTE) != (m_programCounter & HI_BYTE))
      m_cycles++;

    m_programCounter = m_addrAbs;
  }

  return 0;
}

// CLear Carry flag
uint8_t CPU::CLC()
{
  SetFlag(C, false);

  return 0;
}

// CLear Decimal flag
uint8_t CPU::CLD()
{
  SetFlag(D, false);

  return 0;
}

// CLear Interrupt flag
uint8_t CPU::CLI()
{
  SetFlag(I, false);

  return 0;
}


// CLear oVerflow
uint8_t CPU::CLV()
{
  SetFlag(V, false);

  return 0;
}

// CoMPare values
uint8_t CPU::CMP()
{
  Fetch();
  m_temp = (uint16_t)m_a - (uint16_t)m_fetched;

  SetFlag(C, m_a >= m_fetched);
  SetFlag(Z, (m_temp & LO_BYTE) == 0x0000);
  SetFlag(N, m_temp & NEGATIVE_MASK);

  return 1;
}

// ComPare with X index
uint8_t CPU::CPX()
{
  Fetch();
  m_temp = (uint16_t)m_x - (uint16_t)m_fetched;

  SetFlag(C, m_x >= m_fetched);
  SetFlag(Z, (m_temp & LO_BYTE) == 0x0000);
  SetFlag(N, m_temp & NEGATIVE_MASK);

  return 0;
}

// ComPare with Y index
uint8_t CPU::CPY()
{
  Fetch();
  m_temp = (uint16_t)m_y - (uint16_t)m_fetched;

  SetFlag(C, m_y >= m_fetched);
  SetFlag(Z, (m_temp & LO_BYTE) == 0x0000);
  SetFlag(N, m_temp & NEGATIVE_MASK);

  return 0;
}

// DECrement value
uint8_t CPU::DEC()
{
  Fetch();
  m_temp = m_fetched - 1;
  Write(m_addrAbs, m_temp & LO_BYTE);

  SetFlag(Z, (m_temp & LO_BYTE) == 0x0000);
  SetFlag(N, m_temp & NEGATIVE_MASK);

  return 0;
}

// DEcrement X index
uint8_t CPU::DEX()
{
  m_x--;

  SetFlag(Z, m_x == 0x00);
  SetFlag(N, m_x & NEGATIVE_MASK);

  return 0;
}

// DEcrement Y index
uint8_t CPU::DEY()
{
  m_y--;

  SetFlag(Z, m_y == 0x00);
  SetFlag(N, m_y & NEGATIVE_MASK);

  return 0;
}

// Exclusive OR accumulator
uint8_t CPU::EOR()
{
  Fetch();
  m_a = m_a ^ m_fetched;

  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & NEGATIVE_MASK);

  return 1;
}

// INCrement value
uint8_t CPU::INC()
{
  Fetch();
  m_temp = m_fetched + 1;
  Write(m_addrAbs, m_temp & LO_BYTE);

  SetFlag(Z, (m_temp & LO_BYTE) == 0x0000);
  SetFlag(N, m_temp & NEGATIVE_MASK);

  return 0;
}

// INcrement X index
uint8_t CPU::INX()
{
  m_x++;

  SetFlag(Z, m_x == 0x00);
  SetFlag(N, m_x & NEGATIVE_MASK);

  return 0;
}

// INcrement Y index
uint8_t CPU::INY()
{
  m_y++;

  SetFlag(Z, m_y == 0x00);
  SetFlag(N, m_y & NEGATIVE_MASK);

  return 0;
}

// JuMP to a new location
uint8_t CPU::JMP()
{
  m_programCounter = m_addrAbs;

  return 0;
}

// Jump to SubRoutine
uint8_t CPU::JSR()
{
  m_programCounter--;

  Write(0x0100 + m_stackPointer, (m_programCounter >> 8) & LO_BYTE);
  m_stackPointer--;
  Write(0x0100 + m_stackPointer, m_programCounter & LO_BYTE);
  m_stackPointer--;

  m_programCounter = m_addrAbs;

  return 0;
}

// LoaD Accumulator with value
uint8_t CPU::LDA()
{
  Fetch();
  m_a = m_fetched;

  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & NEGATIVE_MASK);

  return 1;
}

// LoaD X index with value
uint8_t CPU::LDX()
{
  Fetch();
  m_x = m_fetched;

  SetFlag(Z, m_x == 0x00);
  SetFlag(N, m_x & NEGATIVE_MASK);

  return 1;
}

// LoaD Y index with value
uint8_t CPU::LDY()
{
  Fetch();
  m_y = m_fetched;

  SetFlag(Z, m_y == 0x00);
  SetFlag(N, m_y & NEGATIVE_MASK);

  return 1;
}

// Logical Shift Right
uint8_t CPU::LSR()
{
  Fetch();
  m_temp = m_fetched >> 1;

  if (m_lookup[m_opcode].addrmode == &CPU::IMP)
    m_a = m_temp & LO_BYTE;
  else
    Write(m_addrAbs, m_temp & LO_BYTE);

  SetFlag(C, m_fetched & CARRY_MASK);
  SetFlag(Z, m_temp == 0x0000);
  SetFlag(N, 0);

  return 0;
}

// No OPeration
uint8_t CPU::NOP()
{
  return 0;
}

// OR with Accumulator
uint8_t CPU::ORA()
{
  Fetch();
  m_a |= m_fetched;

  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & NEGATIVE_MASK);

  return 1;
}

// PusH Accumulator
uint8_t CPU::PHA()
{
  Write(0x0100 + m_stackPointer, m_a);
  m_stackPointer--;

  return 0;
}

// PusH Program flags register
uint8_t CPU::PHP()
{
  Write(0x0100 + m_stackPointer, m_status | B | U);
  m_stackPointer--;

  SetFlag(B, 0);
  SetFlag(U, 0);

  return 0;
}

// PuLl Accumulator
uint8_t CPU::PLA()
{
  m_stackPointer++;
  m_a = Read(0x0100 + m_stackPointer);

  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & NEGATIVE_MASK);

  return 0;
}

// PuLl Program flags register
uint8_t CPU::PLP()
{
  m_stackPointer++;
  m_status = Read(0x0100 + m_stackPointer);

  SetFlag(U, 1);

  return 0;
}

// ROtate bits Left
uint8_t CPU::ROL()
{
  Fetch();
  m_temp = (uint16_t)(m_fetched << 1) | GetFlag(C);

  if (m_lookup[m_opcode].addrmode == &CPU::IMP)
    m_a = m_temp & LO_BYTE;
  else
    Write(m_addrAbs, m_temp & LO_BYTE);

  SetFlag(C, m_temp & HI_BYTE);
  SetFlag(Z, (m_temp & LO_BYTE) == 0x0000);
  SetFlag(N, m_temp & NEGATIVE_MASK);

  return 0;
}

// ROtate bits Right
uint8_t CPU::ROR()
{
  Fetch();
  m_temp = (uint16_t)(GetFlag(C) << 7) | (m_fetched >> 1);

  if (m_lookup[m_opcode].addrmode == &CPU::IMP)
    m_a = m_temp & LO_BYTE;
  else
    Write(m_addrAbs, m_temp & LO_BYTE);

  SetFlag(C, m_fetched & CARRY_MASK);
  SetFlag(Z, (m_temp & LO_BYTE) == 0x00);
  SetFlag(N, m_temp & NEGATIVE_MASK);

  return 0;
}

// ReTurn from Interrupt
uint8_t CPU::RTI()
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

// ReTurn from Subroutine
uint8_t CPU::RTS()
{
  m_stackPointer++;
  m_programCounter = (uint16_t)Read(0x0100 + m_stackPointer);
  m_stackPointer++;
  m_programCounter |= (uint16_t)Read(0x0100 + m_stackPointer) << 8;
  m_programCounter++;

  return 0;
}

// SEt Carry flag
uint8_t CPU::SEC()
{
  SetFlag(C, true);

  return 0;
}

// SEt Decimal flag
uint8_t CPU::SED()
{
  SetFlag(D, true);

  return 0;
}

// SEt Interrupt disable flag
uint8_t CPU::SEI()
{
  SetFlag(I, true);

  return 0;
}

// STore Accumulator into memory
uint8_t CPU::STA()
{
  Write(m_addrAbs, m_a);

  return 0;
}

// STore X index into memory
uint8_t CPU::STX()
{
  Write(m_addrAbs, m_x);

  return 0;
}

// STore Y index into memory
uint8_t CPU::STY()
{
  Write(m_addrAbs, m_y);

  return 0;
}

// Transfer Accumulator to X index
uint8_t CPU::TAX()
{
  m_x = m_a;

  SetFlag(Z, m_x == 0x00);
  SetFlag(N, m_x & NEGATIVE_MASK);

  return 0;
}

// Transfer Accumulator to Y index
uint8_t CPU::TAY()
{
  m_y = m_a;

  SetFlag(Z, m_y == 0x00);
  SetFlag(N, m_y & NEGATIVE_MASK);

  return 0;
}

// Transfer Stack pointer to X index
uint8_t CPU::TSX()
{
  m_x = m_stackPointer;

  SetFlag(Z, m_x == 0x00);
  SetFlag(N, m_x & NEGATIVE_MASK);

  return 0;
}

// Transfer X index to Accumulator
uint8_t CPU::TXA()
{
  m_a = m_x;

  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & NEGATIVE_MASK);

  return 0;
}

// Transfer X index to Stack pointer
uint8_t CPU::TXS()
{
  m_stackPointer = m_x;

  return 0;
}

// Transfer Y index to Accumulator
uint8_t CPU::TYA()
{
  m_a = m_y;

  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & NEGATIVE_MASK);

  return 0;
}

// Load Accumulator and X index
uint8_t CPU::LAX()
{
  Fetch();
  m_a = m_fetched;
  m_x = m_a;

  SetFlag(Z, m_x == 0x00);
  SetFlag(N, m_x & NEGATIVE_MASK);

  return 0;
}

// Stores X index and Accumulator into memory
uint8_t CPU::SAX()
{
  Write(m_addrAbs, m_a & m_x);

  return 0;
}

// Decrement then ComPare
uint8_t CPU::DCP()
{
  // DEC
  Fetch();
  m_temp = m_fetched - 1;
  Write(m_addrAbs, m_temp & LO_BYTE);

  // CMP
  Fetch();
  m_temp = (uint16_t)m_a - (uint16_t)m_fetched;

  SetFlag(C, m_a >= m_fetched);
  SetFlag(Z, (m_temp & LO_BYTE) == 0x0000);
  SetFlag(N, m_temp & NEGATIVE_MASK);

  return 1;
}

// Shift Right then Eor
uint8_t CPU::SRE()
{
  // LSR
  Fetch();
  m_temp = m_fetched >> 1;

  if (m_lookup[m_opcode].addrmode == &CPU::IMP)
    m_a = m_temp & LO_BYTE;
  else
    Write(m_addrAbs, m_temp & LO_BYTE);

  SetFlag(C, m_fetched & 0x0001);

  // EOR
  Fetch();
  m_a = m_a ^ m_fetched;

  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & NEGATIVE_MASK);

  return 1;
}

// Rotate the value Right, then Add w/ carry flag to the accumulator
uint8_t CPU::RRA()
{
  // ROR
  Fetch();
  m_temp = (uint16_t)(GetFlag(C) << 7) | (m_fetched >> 1);

  if (m_lookup[m_opcode].addrmode == &CPU::IMP)
    m_a = m_temp & LO_BYTE;
  else
    Write(m_addrAbs, m_temp & LO_BYTE);

  SetFlag(C, m_fetched & CARRY_MASK);

  // ADC
  Fetch();
  m_temp = (uint16_t)m_a + (uint16_t)m_fetched + (uint16_t)GetFlag(C);

  SetFlag(C, m_temp > 255);
  SetFlag(Z, (m_temp & LO_BYTE) == 0);
  SetFlag(V, (~((uint16_t)m_a ^ (uint16_t)m_fetched) & ((uint16_t)m_a ^ (uint16_t)m_temp)) & NEGATIVE_MASK);
  SetFlag(N, m_temp & NEGATIVE_MASK);

  m_a = m_temp & LO_BYTE;

  return 1;
}

// Rotate Left then And
uint8_t CPU::RLA()
{
  // ROL
  Fetch();
  m_temp = (uint16_t)(m_fetched << 1) | GetFlag(C);

  if (m_lookup[m_opcode].addrmode == &CPU::IMP)
    m_a = m_temp & LO_BYTE;
  else
    Write(m_addrAbs, m_temp & LO_BYTE);

  SetFlag(C, m_temp & HI_BYTE);

  // AND
  Fetch();
  m_a = m_a & m_fetched;

  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & NEGATIVE_MASK);

  return 1;
}

// Increment then Subtract with Carry
uint8_t CPU::ISC()
{
  // INC
  Fetch();
  m_temp = m_fetched + 1;
  Write(m_addrAbs, m_temp & LO_BYTE);

  // SBC
  Fetch();
  uint16_t value = ((uint16_t)m_fetched) ^ LO_BYTE;
  m_temp = (uint16_t)m_a + value + (uint16_t)GetFlag(C);

  SetFlag(C, m_temp & HI_BYTE);
  SetFlag(Z, ((m_temp & LO_BYTE) == 0));
  SetFlag(V, (m_temp ^ (uint16_t)m_a) & (m_temp ^ value) & NEGATIVE_MASK);
  SetFlag(N, m_temp & NEGATIVE_MASK);

  m_a = m_temp & LO_BYTE;

  return 1;
}

// Shift Left then Or
uint8_t CPU::SLO()
{
  // ASL
  Fetch();
  m_temp = (uint16_t)m_fetched << 1;

  if (m_lookup[m_opcode].addrmode == &CPU::IMP)
    m_a = m_temp & LO_BYTE;
  else
    Write(m_addrAbs, m_temp & LO_BYTE);

  SetFlag(C, (m_temp & HI_BYTE) > 0);

  // ORA
  Fetch();
  m_a |= m_fetched;

  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & NEGATIVE_MASK);

  return 1;
}

// KILl execution
uint8_t CPU::KIL()
{
  // TODO: block CPU
  return 0;
}

// Not implemented
uint8_t CPU::XXX()
{
  return 0;
}
