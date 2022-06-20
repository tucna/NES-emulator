#include "Bus.h"
#include "Cpu.h"

Cpu::Cpu() :
  m_bus(nullptr),
  m_status(0),
  m_fetched(0),
  m_addrAbs(0),
  m_addrRel(0),
  m_opcode(0),
  m_temp(0),
  m_cycles(0),
  m_clockCount(0)
{
  // Basic version taken from javidx9 - thanks!
  // {opcode, mnemonic, function pointer, address mode pointer, number clock cycles}
  m_lookup =
  {
    { 0x00, "BRK", &Cpu::BRK, &Cpu::IMM, 7 },{ 0x01, "ORA", &Cpu::ORA, &Cpu::IZX, 6 },{ 0x02, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0x03, "???", &Cpu::XXX, &Cpu::IMP, 8 },{ 0x04, "NOP", &Cpu::NOP, &Cpu::ZP0, 3 },{ 0x05, "ORA", &Cpu::ORA, &Cpu::ZP0, 3 },{ 0x06, "ASL", &Cpu::ASL, &Cpu::ZP0, 5 },{ 0x07, "???", &Cpu::XXX, &Cpu::IMP, 5 },{ 0x08, "PHP", &Cpu::PHP, &Cpu::IMP, 3 },{ 0x09, "ORA", &Cpu::ORA, &Cpu::IMM, 2 },{ 0x0A, "ASL", &Cpu::ASL, &Cpu::IMP, 2 },{ 0x0B, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0x0C, "NOP", &Cpu::NOP, &Cpu::ABS, 4 },{ 0x0D, "ORA", &Cpu::ORA, &Cpu::ABS, 4 },{ 0x0E, "ASL", &Cpu::ASL, &Cpu::ABS, 6 },{ 0x0F, "???", &Cpu::XXX, &Cpu::IMP, 6 },
    { 0x10, "BPL", &Cpu::BPL, &Cpu::REL, 2 },{ 0x11, "ORA", &Cpu::ORA, &Cpu::IZY, 5 },{ 0x12, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0x13, "???", &Cpu::XXX, &Cpu::IMP, 8 },{ 0x14, "NOP", &Cpu::NOP, &Cpu::ZPX, 4 },{ 0x15, "ORA", &Cpu::ORA, &Cpu::ZPX, 4 },{ 0x16, "ASL", &Cpu::ASL, &Cpu::ZPX, 6 },{ 0x17, "???", &Cpu::XXX, &Cpu::IMP, 6 },{ 0x18, "CLC", &Cpu::CLC, &Cpu::IMP, 2 },{ 0x19, "ORA", &Cpu::ORA, &Cpu::ABY, 4 },{ 0x1A, "NOP", &Cpu::NOP, &Cpu::IMP, 2 },{ 0x1B, "???", &Cpu::XXX, &Cpu::IMP, 7 },{ 0x1C, "NOP", &Cpu::NOP, &Cpu::ABX, 4 },{ 0x1D, "ORA", &Cpu::ORA, &Cpu::ABX, 4 },{ 0x1E, "ASL", &Cpu::ASL, &Cpu::ABX, 7 },{ 0x1F, "???", &Cpu::XXX, &Cpu::IMP, 7 },
    { 0x20, "JSR", &Cpu::JSR, &Cpu::ABS, 6 },{ 0x21, "AND", &Cpu::AND, &Cpu::IZX, 6 },{ 0x22, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0x23, "RLA", &Cpu::RLA, &Cpu::IZX, 8 },{ 0x24, "BIT", &Cpu::BIT, &Cpu::ZP0, 3 },{ 0x25, "AND", &Cpu::AND, &Cpu::ZP0, 3 },{ 0x26, "ROL", &Cpu::ROL, &Cpu::ZP0, 5 },{ 0x27, "RLA", &Cpu::RLA, &Cpu::ZP0, 5 },{ 0x28, "PLP", &Cpu::PLP, &Cpu::IMP, 4 },{ 0x29, "AND", &Cpu::AND, &Cpu::IMM, 2 },{ 0x2A, "ROL", &Cpu::ROL, &Cpu::IMP, 2 },{ 0x2B, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0x2C, "BIT", &Cpu::BIT, &Cpu::ABS, 4 },{ 0x2D, "AND", &Cpu::AND, &Cpu::ABS, 4 },{ 0x2E, "ROL", &Cpu::ROL, &Cpu::ABS, 6 },{ 0x2F, "RLA", &Cpu::RLA, &Cpu::ABS, 6 },
    { 0x30, "BMI", &Cpu::BMI, &Cpu::REL, 2 },{ 0x31, "AND", &Cpu::AND, &Cpu::IZY, 5 },{ 0x32, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0x33, "RLA", &Cpu::RLA, &Cpu::IZY, 8 },{ 0x34, "NOP", &Cpu::NOP, &Cpu::ZPX, 4 },{ 0x35, "AND", &Cpu::AND, &Cpu::ZPX, 4 },{ 0x36, "ROL", &Cpu::ROL, &Cpu::ZPX, 6 },{ 0x37, "RLA", &Cpu::RLA, &Cpu::ZPX, 6 },{ 0x38, "SEC", &Cpu::SEC, &Cpu::IMP, 2 },{ 0x39, "AND", &Cpu::AND, &Cpu::ABY, 4 },{ 0x3A, "NOP", &Cpu::NOP, &Cpu::IMP, 2 },{ 0x3B, "RLA", &Cpu::RLA, &Cpu::ABY, 7 },{ 0x3C, "NOP", &Cpu::NOP, &Cpu::ABX, 4 },{ 0x3D, "AND", &Cpu::AND, &Cpu::ABX, 4 },{ 0x3E, "ROL", &Cpu::ROL, &Cpu::ABX, 7 },{ 0x3F, "RLA", &Cpu::RLA, &Cpu::ABX, 7 },
    { 0x40, "RTI", &Cpu::RTI, &Cpu::IMP, 6 },{ 0x41, "EOR", &Cpu::EOR, &Cpu::IZX, 6 },{ 0x42, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0x43, "SRE", &Cpu::SRE, &Cpu::IZX, 8 },{ 0x44, "NOP", &Cpu::NOP, &Cpu::ZP0, 3 },{ 0x45, "EOR", &Cpu::EOR, &Cpu::ZP0, 3 },{ 0x46, "LSR", &Cpu::LSR, &Cpu::ZP0, 5 },{ 0x47, "SRE", &Cpu::SRE, &Cpu::ZP0, 5 },{ 0x48, "PHA", &Cpu::PHA, &Cpu::IMP, 3 },{ 0x49, "EOR", &Cpu::EOR, &Cpu::IMM, 2 },{ 0x4A, "LSR", &Cpu::LSR, &Cpu::IMP, 2 },{ 0x4B, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0x4C, "JMP", &Cpu::JMP, &Cpu::ABS, 3 },{ 0x4D, "EOR", &Cpu::EOR, &Cpu::ABS, 4 },{ 0x4E, "LSR", &Cpu::LSR, &Cpu::ABS, 6 },{ 0x4F, "SRE", &Cpu::SRE, &Cpu::ABS, 6 },
    { 0x50, "BVC", &Cpu::BVC, &Cpu::REL, 2 },{ 0x51, "EOR", &Cpu::EOR, &Cpu::IZY, 5 },{ 0x52, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0x53, "SRE", &Cpu::SRE, &Cpu::IZY, 8 },{ 0x54, "NOP", &Cpu::NOP, &Cpu::ZPX, 4 },{ 0x55, "EOR", &Cpu::EOR, &Cpu::ZPX, 4 },{ 0x56, "LSR", &Cpu::LSR, &Cpu::ZPX, 6 },{ 0x57, "SRE", &Cpu::SRE, &Cpu::ZPX, 6 },{ 0x58, "CLI", &Cpu::CLI, &Cpu::IMP, 2 },{ 0x59, "EOR", &Cpu::EOR, &Cpu::ABY, 4 },{ 0x5A, "NOP", &Cpu::NOP, &Cpu::IMP, 2 },{ 0x5B, "SRE", &Cpu::SRE, &Cpu::ABY, 7 },{ 0x5C, "NOP", &Cpu::NOP, &Cpu::ABX, 4 },{ 0x5D, "EOR", &Cpu::EOR, &Cpu::ABX, 4 },{ 0x5E, "LSR", &Cpu::LSR, &Cpu::ABX, 7 },{ 0x5F, "SRE", &Cpu::SRE, &Cpu::ABX, 7 },
    { 0x60, "RTS", &Cpu::RTS, &Cpu::IMP, 6 },{ 0x61, "ADC", &Cpu::ADC, &Cpu::IZX, 6 },{ 0x62, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0x63, "RRA", &Cpu::RRA, &Cpu::IZX, 8 },{ 0x64, "NOP", &Cpu::NOP, &Cpu::ZP0, 3 },{ 0x65, "ADC", &Cpu::ADC, &Cpu::ZP0, 3 },{ 0x66, "ROR", &Cpu::ROR, &Cpu::ZP0, 5 },{ 0x67, "RRA", &Cpu::RRA, &Cpu::ZP0, 5 },{ 0x68, "PLA", &Cpu::PLA, &Cpu::IMP, 4 },{ 0x69, "ADC", &Cpu::ADC, &Cpu::IMM, 2 },{ 0x6A, "ROR", &Cpu::ROR, &Cpu::IMP, 2 },{ 0x6B, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0x6C, "JMP", &Cpu::JMP, &Cpu::IND, 5 },{ 0x6D, "ADC", &Cpu::ADC, &Cpu::ABS, 4 },{ 0x6E, "ROR", &Cpu::ROR, &Cpu::ABS, 6 },{ 0x6F, "RRA", &Cpu::RRA, &Cpu::ABS, 6 },
    { 0x70, "BVS", &Cpu::BVS, &Cpu::REL, 2 },{ 0x71, "ADC", &Cpu::ADC, &Cpu::IZY, 5 },{ 0x72, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0x73, "RRA", &Cpu::RRA, &Cpu::IZY, 8 },{ 0x74, "NOP", &Cpu::NOP, &Cpu::ZPX, 4 },{ 0x75, "ADC", &Cpu::ADC, &Cpu::ZPX, 4 },{ 0x76, "ROR", &Cpu::ROR, &Cpu::ZPX, 6 },{ 0x77, "RRA", &Cpu::RRA, &Cpu::ZPX, 6 },{ 0x78, "SEI", &Cpu::SEI, &Cpu::IMP, 2 },{ 0x79, "ADC", &Cpu::ADC, &Cpu::ABY, 4 },{ 0x7A, "NOP", &Cpu::NOP, &Cpu::IMP, 2 },{ 0x7B, "RRA", &Cpu::RRA, &Cpu::ABY, 7 },{ 0x7C, "NOP", &Cpu::NOP, &Cpu::ABX, 4 },{ 0x7D, "ADC", &Cpu::ADC, &Cpu::ABX, 4 },{ 0x7E, "ROR", &Cpu::ROR, &Cpu::ABX, 7 },{ 0x7F, "RRA", &Cpu::RRA, &Cpu::ABX, 7 },
    { 0x80, "NOP", &Cpu::NOP, &Cpu::IMM, 2 },{ 0x81, "STA", &Cpu::STA, &Cpu::IZX, 6 },{ 0x82, "NOP", &Cpu::NOP, &Cpu::IMM, 2 },{ 0x83, "SAX", &Cpu::SAX, &Cpu::IZX, 6 },{ 0x84, "STY", &Cpu::STY, &Cpu::ZP0, 3 },{ 0x85, "STA", &Cpu::STA, &Cpu::ZP0, 3 },{ 0x86, "STX", &Cpu::STX, &Cpu::ZP0, 3 },{ 0x87, "SAX", &Cpu::SAX, &Cpu::ZP0, 3 },{ 0x88, "DEY", &Cpu::DEY, &Cpu::IMP, 2 },{ 0x89, "NOP", &Cpu::NOP, &Cpu::IMM, 2 },{ 0x8A, "TXA", &Cpu::TXA, &Cpu::IMP, 2 },{ 0x8B, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0x8C, "STY", &Cpu::STY, &Cpu::ABS, 4 },{ 0x8D, "STA", &Cpu::STA, &Cpu::ABS, 4 },{ 0x8E, "STX", &Cpu::STX, &Cpu::ABS, 4 },{ 0x8F, "SAX", &Cpu::SAX, &Cpu::ABS, 4 },
    { 0x90, "BCC", &Cpu::BCC, &Cpu::REL, 2 },{ 0x91, "STA", &Cpu::STA, &Cpu::IZY, 6 },{ 0x92, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0x93, "???", &Cpu::XXX, &Cpu::IMP, 6 },{ 0x94, "STY", &Cpu::STY, &Cpu::ZPX, 4 },{ 0x95, "STA", &Cpu::STA, &Cpu::ZPX, 4 },{ 0x96, "STX", &Cpu::STX, &Cpu::ZPY, 4 },{ 0x97, "SAX", &Cpu::SAX, &Cpu::ZPY, 4 },{ 0x98, "TYA", &Cpu::TYA, &Cpu::IMP, 2 },{ 0x99, "STA", &Cpu::STA, &Cpu::ABY, 5 },{ 0x9A, "TXS", &Cpu::TXS, &Cpu::IMP, 2 },{ 0x9B, "???", &Cpu::XXX, &Cpu::IMP, 5 },{ 0x9C, "???", &Cpu::XXX, &Cpu::IMP, 5 },{ 0x9D, "STA", &Cpu::STA, &Cpu::ABX, 5 },{ 0x9E, "???", &Cpu::XXX, &Cpu::IMP, 5 },{ 0x9F, "???", &Cpu::XXX, &Cpu::IMP, 5 },
    { 0xA0, "LDY", &Cpu::LDY, &Cpu::IMM, 2 },{ 0xA1, "LDA", &Cpu::LDA, &Cpu::IZX, 6 },{ 0xA2, "LDX", &Cpu::LDX, &Cpu::IMM, 2 },{ 0xA3, "LAX", &Cpu::LAX, &Cpu::IZX, 6 },{ 0xA4, "LDY", &Cpu::LDY, &Cpu::ZP0, 3 },{ 0xA5, "LDA", &Cpu::LDA, &Cpu::ZP0, 3 },{ 0xA6, "LDX", &Cpu::LDX, &Cpu::ZP0, 3 },{ 0xA7, "LAX", &Cpu::LAX, &Cpu::ZP0, 3 },{ 0xA8, "TAY", &Cpu::TAY, &Cpu::IMP, 2 },{ 0xA9, "LDA", &Cpu::LDA, &Cpu::IMM, 2 },{ 0xAA, "TAX", &Cpu::TAX, &Cpu::IMP, 2 },{ 0xAB, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0xAC, "LDY", &Cpu::LDY, &Cpu::ABS, 4 },{ 0xAD, "LDA", &Cpu::LDA, &Cpu::ABS, 4 },{ 0xAE, "LDX", &Cpu::LDX, &Cpu::ABS, 4 },{ 0xAF, "LAX", &Cpu::LAX, &Cpu::ABS, 4 },
    { 0xB0, "BCS", &Cpu::BCS, &Cpu::REL, 2 },{ 0xB1, "LDA", &Cpu::LDA, &Cpu::IZY, 5 },{ 0xB2, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0xB3, "LAX", &Cpu::LAX, &Cpu::IZY, 5 },{ 0xB4, "LDY", &Cpu::LDY, &Cpu::ZPX, 4 },{ 0xB5, "LDA", &Cpu::LDA, &Cpu::ZPX, 4 },{ 0xB6, "LDX", &Cpu::LDX, &Cpu::ZPY, 4 },{ 0xB7, "LAX", &Cpu::LAX, &Cpu::ZPY, 4 },{ 0xB8, "CLV", &Cpu::CLV, &Cpu::IMP, 2 },{ 0xB9, "LDA", &Cpu::LDA, &Cpu::ABY, 4 },{ 0xBA, "TSX", &Cpu::TSX, &Cpu::IMP, 2 },{ 0xBB, "???", &Cpu::XXX, &Cpu::IMP, 4 },{ 0xBC, "LDY", &Cpu::LDY, &Cpu::ABX, 4 },{ 0xBD, "LDA", &Cpu::LDA, &Cpu::ABX, 4 },{ 0xBE, "LDX", &Cpu::LDX, &Cpu::ABY, 4 },{ 0xBF, "LAX", &Cpu::LAX, &Cpu::ABY, 4 },
    { 0xC0, "CPY", &Cpu::CPY, &Cpu::IMM, 2 },{ 0xC1, "CMP", &Cpu::CMP, &Cpu::IZX, 6 },{ 0xC2, "NOP", &Cpu::NOP, &Cpu::IMM, 2 },{ 0xC3, "DCP", &Cpu::DCP, &Cpu::IZX, 8 },{ 0xC4, "CPY", &Cpu::CPY, &Cpu::ZP0, 3 },{ 0xC5, "CMP", &Cpu::CMP, &Cpu::ZP0, 3 },{ 0xC6, "DEC", &Cpu::DEC, &Cpu::ZP0, 5 },{ 0xC7, "DCP", &Cpu::DCP, &Cpu::ZP0, 5 },{ 0xC8, "INY", &Cpu::INY, &Cpu::IMP, 2 },{ 0xC9, "CMP", &Cpu::CMP, &Cpu::IMM, 2 },{ 0xCA, "DEX", &Cpu::DEX, &Cpu::IMP, 2 },{ 0xCB, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0xCC, "CPY", &Cpu::CPY, &Cpu::ABS, 4 },{ 0xCD, "CMP", &Cpu::CMP, &Cpu::ABS, 4 },{ 0xCE, "DEC", &Cpu::DEC, &Cpu::ABS, 6 },{ 0xCF, "DCP", &Cpu::DCP, &Cpu::ABS, 6 },
    { 0xD0, "BNE", &Cpu::BNE, &Cpu::REL, 2 },{ 0xD1, "CMP", &Cpu::CMP, &Cpu::IZY, 5 },{ 0xD2, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0xD3, "DCP", &Cpu::DCP, &Cpu::IZY, 8 },{ 0xD4, "NOP", &Cpu::NOP, &Cpu::ZPX, 4 },{ 0xD5, "CMP", &Cpu::CMP, &Cpu::ZPX, 4 },{ 0xD6, "DEC", &Cpu::DEC, &Cpu::ZPX, 6 },{ 0xD7, "DCP", &Cpu::DCP, &Cpu::ZPX, 6 },{ 0xD8, "CLD", &Cpu::CLD, &Cpu::IMP, 2 },{ 0xD9, "CMP", &Cpu::CMP, &Cpu::ABY, 4 },{ 0xDA, "NOP", &Cpu::NOP, &Cpu::IMP, 2 },{ 0xDB, "DCP", &Cpu::DCP, &Cpu::ABY, 7 },{ 0xDC, "NOP", &Cpu::NOP, &Cpu::ABX, 4 },{ 0xDD, "CMP", &Cpu::CMP, &Cpu::ABX, 4 },{ 0xDE, "DEC", &Cpu::DEC, &Cpu::ABX, 7 },{ 0xDF, "DCP", &Cpu::DCP, &Cpu::ABX, 7 },
    { 0xE0, "CPX", &Cpu::CPX, &Cpu::IMM, 2 },{ 0xE1, "SBC", &Cpu::SBC, &Cpu::IZX, 6 },{ 0xE2, "NOP", &Cpu::NOP, &Cpu::IMM, 2 },{ 0xE3, "ISC", &Cpu::ISC, &Cpu::IZX, 8 },{ 0xE4, "CPX", &Cpu::CPX, &Cpu::ZP0, 3 },{ 0xE5, "SBC", &Cpu::SBC, &Cpu::ZP0, 3 },{ 0xE6, "INC", &Cpu::INC, &Cpu::ZP0, 5 },{ 0xE7, "ISC", &Cpu::ISC, &Cpu::ZP0, 5 },{ 0xE8, "INX", &Cpu::INX, &Cpu::IMP, 2 },{ 0xE9, "SBC", &Cpu::SBC, &Cpu::IMM, 2 },{ 0xEA, "NOP", &Cpu::NOP, &Cpu::IMP, 2 },{ 0xEB, "SBC", &Cpu::SBC, &Cpu::IMM, 2 },{ 0xEC, "CPX", &Cpu::CPX, &Cpu::ABS, 4 },{ 0xED, "SBC", &Cpu::SBC, &Cpu::ABS, 4 },{ 0xEE, "INC", &Cpu::INC, &Cpu::ABS, 6 },{ 0xEF, "ISC", &Cpu::ISC, &Cpu::ABS, 6 },
    { 0xF0, "BEQ", &Cpu::BEQ, &Cpu::REL, 2 },{ 0xF1, "SBC", &Cpu::SBC, &Cpu::IZY, 5 },{ 0xF2, "???", &Cpu::XXX, &Cpu::IMP, 2 },{ 0xF3, "ISC", &Cpu::ISC, &Cpu::IZY, 8 },{ 0xF4, "NOP", &Cpu::NOP, &Cpu::ZPX, 4 },{ 0xF5, "SBC", &Cpu::SBC, &Cpu::ZPX, 4 },{ 0xF6, "INC", &Cpu::INC, &Cpu::ZPX, 6 },{ 0xF7, "ISC", &Cpu::ISC, &Cpu::ZPX, 6 },{ 0xF8, "SED", &Cpu::SED, &Cpu::IMP, 2 },{ 0xF9, "SBC", &Cpu::SBC, &Cpu::ABY, 4 },{ 0xFA, "NOP", &Cpu::NOP, &Cpu::IMP, 2 },{ 0xFB, "ISC", &Cpu::ISC, &Cpu::ABY, 7 },{ 0xFC, "NOP", &Cpu::NOP, &Cpu::ABX, 4 },{ 0xFD, "SBC", &Cpu::SBC, &Cpu::ABX, 4 },{ 0xFE, "INC", &Cpu::INC, &Cpu::ABX, 7 },{ 0xFF, "ISC", &Cpu::ISC, &Cpu::ABX, 7 },
  };
}

void Cpu::Reset()
{
  // Get address to set program counter to
  m_addrAbs = 0xFFFC;
  uint16_t lo = Read(m_addrAbs + 0);
  uint16_t hi = Read(m_addrAbs + 1);

  // Set it
  m_programCounter = (hi << 8) | lo;

  // Reset internal registers
  m_a = 0;
  m_x = 0;
  m_y = 0;
  m_stackPointer = 0xFD;
  m_status = 0x00 | U;

  // Clear internal helper variables
  m_addrRel = 0x0000;
  m_addrAbs = 0x0000;
  m_fetched = 0x00;

  // Reset takes time
  m_cycles = 8;
}

void Cpu::Clock()
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

void Cpu::NMI()
{
  Write(0x0100 + m_stackPointer, (m_programCounter >> 8) & 0x00FF);
  m_stackPointer--;
  Write(0x0100 + m_stackPointer, m_programCounter & 0x00FF);
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

std::map<uint16_t, std::string> Cpu::Disassemble(uint16_t addrStart, uint16_t addrStop)
{
  uint8_t value = 0x00;
  uint8_t lo = 0x00;
  uint8_t hi = 0x00;
  uint16_t lineAddr = 0;
  uint32_t addr = addrStart;

  std::map<uint16_t, std::string> mapLines;

  auto hex = [](uint32_t n, uint8_t d)
  {
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
      s[i] = "0123456789ABCDEF"[n & 0xF];

    return s;
  };

  while (addr <= (uint32_t)addrStop)
  {
    lineAddr = addr;

    // Prefix line with instruction address
    std::string sInst = "$" + hex(addr, 4) + ": ";

    // Read instruction, and get its readable name
    uint8_t opcode = m_bus->Read(addr, true); addr++;
    sInst += m_lookup[opcode].name + " ";

    if (m_lookup[opcode].addrmode == &Cpu::IMP)
    {
      sInst += " {IMP}";
    }
    else if (m_lookup[opcode].addrmode == &Cpu::IMM)
    {
      value = m_bus->Read(addr, true); addr++;
      sInst += "#$" + hex(value, 2) + " {IMM}";
    }
    else if (m_lookup[opcode].addrmode == &Cpu::ZP0)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = 0x00;
      sInst += "$" + hex(lo, 2) + " {ZP0}";
    }
    else if (m_lookup[opcode].addrmode == &Cpu::ZPX)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = 0x00;
      sInst += "$" + hex(lo, 2) + ", X {ZPX}";
    }
    else if (m_lookup[opcode].addrmode == &Cpu::ZPY)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = 0x00;
      sInst += "$" + hex(lo, 2) + ", Y {ZPY}";
    }
    else if (m_lookup[opcode].addrmode == &Cpu::IZX)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = 0x00;
      sInst += "($" + hex(lo, 2) + ", X) {IZX}";
    }
    else if (m_lookup[opcode].addrmode == &Cpu::IZY)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = 0x00;
      sInst += "($" + hex(lo, 2) + "), Y {IZY}";
    }
    else if (m_lookup[opcode].addrmode == &Cpu::ABS)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = m_bus->Read(addr, true); addr++;
      sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + " {ABS}";
    }
    else if (m_lookup[opcode].addrmode == &Cpu::ABX)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = m_bus->Read(addr, true); addr++;
      sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", X {ABX}";
    }
    else if (m_lookup[opcode].addrmode == &Cpu::ABY)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = m_bus->Read(addr, true); addr++;
      sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", Y {ABY}";
    }
    else if (m_lookup[opcode].addrmode == &Cpu::IND)
    {
      lo = m_bus->Read(addr, true); addr++;
      hi = m_bus->Read(addr, true); addr++;
      sInst += "($" + hex((uint16_t)(hi << 8) | lo, 4) + ") {IND}";
    }
    else if (m_lookup[opcode].addrmode == &Cpu::REL)
    {
      value = m_bus->Read(addr, true); addr++;
      sInst += "$" + hex(value, 2) + " [$" + hex(addr + (int8_t)value, 4) + "] {REL}";
    }

    mapLines[lineAddr] = sInst;
  }

  return mapLines;
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

// TODO
uint8_t Cpu::LSR()
{
  Fetch();
  SetFlag(C, m_fetched & 0x0001);
  m_temp = m_fetched >> 1;
  SetFlag(Z, m_temp == 0x0000);
  SetFlag(N, 0x0000); // Should not be N = 0?
  if (m_lookup[m_opcode].addrmode == &Cpu::IMP)
    m_a = m_temp & 0x00FF;
  else
    Write(m_addrAbs, m_temp & 0x00FF);

  return 0;
}

// TODO
uint8_t Cpu::NOP()
{
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

// TODO
uint8_t Cpu::LAX()
{
  Fetch();
  m_a = m_fetched;
  m_x = m_a;

  SetFlag(Z, m_x == 0x00);
  SetFlag(N, m_x & 0x80);

  return 0;
}

// TODO
uint8_t Cpu::SAX()
{
  Write(m_addrAbs, m_a & m_x);
  return 0;
}

// TODO
// Decrement then ComPare
uint8_t Cpu::DCP()
{
  // DEC
  Fetch();
  m_temp = m_fetched - 1;
  Write(m_addrAbs, m_temp & 0x00FF);

  // CMP
  Fetch();
  m_temp = (uint16_t)m_a - (uint16_t)m_fetched;

  SetFlag(C, m_a >= m_fetched);
  SetFlag(Z, (m_temp & 0x00FF) == 0x0000);
  SetFlag(N, m_temp & 0x0080);

  return 1;
}

// TODO
// Shift Right then Eor
uint8_t Cpu::SRE()
{
  // LSR
  Fetch();
  m_temp = m_fetched >> 1;
  if (m_lookup[m_opcode].addrmode == &Cpu::IMP)
    m_a = m_temp & 0x00FF;
  else
    Write(m_addrAbs, m_temp & 0x00FF);

  SetFlag(C, m_fetched & 0x0001);

  // EOR
  Fetch();
  m_a = m_a ^ m_fetched;

  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & 0x80);

  return 1;
}

// TODO
// Rotate the value Right, then Add w/ carry flag to the accumulator
uint8_t Cpu::RRA()
{
  // ROR
  Fetch();
  m_temp = (uint16_t)(GetFlag(C) << 7) | (m_fetched >> 1);
  if (m_lookup[m_opcode].addrmode == &Cpu::IMP)
    m_a = m_temp & 0x00FF;
  else
    Write(m_addrAbs, m_temp & 0x00FF);

  SetFlag(C, m_fetched & 0x01);

  // ADC
  Fetch();
  m_temp = (uint16_t)m_a + (uint16_t)m_fetched + (uint16_t)GetFlag(C);
  SetFlag(C, m_temp > 255);
  SetFlag(Z, (m_temp & 0x00FF) == 0);
  SetFlag(V, (~((uint16_t)m_a ^ (uint16_t)m_fetched) & ((uint16_t)m_a ^ (uint16_t)m_temp)) & 0x0080);
  SetFlag(N, m_temp & 0x80);

  m_a = m_temp & 0x00FF;

  return 1;
}

// TODO
// Rotate Left then And
uint8_t Cpu::RLA()
{
  // ROL
  Fetch();
  m_temp = (uint16_t)(m_fetched << 1) | GetFlag(C);
  if (m_lookup[m_opcode].addrmode == &Cpu::IMP)
    m_a = m_temp & 0x00FF;
  else
    Write(m_addrAbs, m_temp & 0x00FF);

  SetFlag(C, m_temp & 0xFF00);

  // AND
  Fetch();
  m_a = m_a & m_fetched;
  SetFlag(Z, m_a == 0x00);
  SetFlag(N, m_a & 0x80);

  return 1;
}

// TODO
// Increment then Subtract with Carry
uint8_t Cpu::ISC()
{
  // INC
  Fetch();
  m_temp = m_fetched + 1;
  Write(m_addrAbs, m_temp & 0x00FF);

  // SBC
  Fetch();
  uint16_t value = ((uint16_t)m_fetched) ^ 0x00FF;
  m_temp = (uint16_t)m_a + value + (uint16_t)GetFlag(C);
  SetFlag(C, m_temp & 0xFF00);
  SetFlag(Z, ((m_temp & 0x00FF) == 0));
  SetFlag(V, (m_temp ^ (uint16_t)m_a) & (m_temp ^ value) & 0x0080);
  SetFlag(N, m_temp & 0x0080);

  m_a = m_temp & 0x00FF;

  return 1;
}

// This function captures illegal opcodes
uint8_t Cpu::XXX()
{
  return 0;
}
