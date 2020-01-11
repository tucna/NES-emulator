#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

/* CPU memory map
$0000 - $07FF	$0800	2KB internal RAM
$0800 - $0FFF	$0800	Mirrors of $0000 - $07FF
$1000 - $17FF	$0800
$1800 - $1FFF	$0800
$2000 - $2007	$0008	NES PPU registers
$2008 - $3FFF	$1FF8	Mirrors of $2000 - 2007 (repeats every 8 bytes)
$4000 - $4017	$0018	NES APU and I / O registers
$4018 - $401F	$0008	APU and I / O functionality that is normally disabled.See CPU Test Mode.
$4020 - $FFFF	$BFE0	Cartridge space : PRG ROM, PRG RAM, and mapper registers(See Note)
*/

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

  Cpu();
  ~Cpu() {}

  void Reset();
  void NMI();
  void Clock();

  void ConnectToBus(Bus* bus) { m_bus = bus; }

  std::map<uint16_t, std::string> Disassemble(uint16_t addrStart, uint16_t addrStop);

  bool GetB0() const { return m_status & Flags::C; }
  bool GetB1() const { return m_status & Flags::Z; }
  bool GetB2() const { return m_status & Flags::I; }
  bool GetB3() const { return m_status & Flags::D; }
  bool GetB4() const { return m_status & Flags::B; }
  bool GetB5() const { return m_status & Flags::U; }
  bool GetB6() const { return m_status & Flags::V; }
  bool GetB7() const { return m_status & Flags::N; }

  bool IsCompleted() { return m_cycles == 0; }

  uint8_t GetAcc() const { return m_a; }
  uint8_t GetRegX() const { return m_x; }
  uint8_t GetRegY() const { return m_y; }
  uint8_t GetStakPointer() const { return m_stackPointer; }

  uint16_t GetProgramCounter() const { return m_programCounter; }

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

  // Opcodes
  uint8_t ADC();	uint8_t AND();	uint8_t ASL();	uint8_t BCC();
  uint8_t BCS();	uint8_t BEQ();	uint8_t BIT();	uint8_t BMI();
  uint8_t BNE();	uint8_t BPL();	uint8_t BRK();	uint8_t BVC();
  uint8_t BVS();	uint8_t CLC();	uint8_t CLD();	uint8_t CLI();
  uint8_t CLV();	uint8_t CMP();	uint8_t CPX();	uint8_t CPY();
  uint8_t DEC();	uint8_t DEX();	uint8_t DEY();	uint8_t EOR();
  uint8_t INC();	uint8_t INX();	uint8_t INY();	uint8_t JMP();
  uint8_t JSR();	uint8_t LDA();	uint8_t LDX();	uint8_t LDY();
  uint8_t LSR();	uint8_t NOP();	uint8_t ORA();	uint8_t PHA();
  uint8_t PHP();	uint8_t PLA();	uint8_t PLP();	uint8_t ROL();
  uint8_t ROR();	uint8_t RTI();	uint8_t RTS();	uint8_t SBC();
  uint8_t SEC();	uint8_t SED();	uint8_t SEI();	uint8_t STA();
  uint8_t STX();	uint8_t STY();	uint8_t TAX();	uint8_t TAY();
  uint8_t TSX();	uint8_t TXA();	uint8_t TXS();	uint8_t TYA();

  // Unofficial opcode
  uint8_t XXX();

  void Write(uint16_t addr, uint8_t data);
  uint8_t Read(uint16_t addr);

  uint8_t Fetch();
  uint8_t GetFlag(Flags flag);
  void SetFlag(Flags flag, bool setClear);

  // Registers
  uint8_t m_a;
  uint8_t m_x;
  uint8_t m_y;
  uint8_t m_stackPointer;
  uint16_t m_programCounter;

  // Capture internal CPU status
  uint8_t m_status;

  uint8_t m_fetched;
  uint8_t m_opcode;
  uint8_t m_cycles;
  uint16_t m_addrAbs;
  uint16_t m_addrRel;
  uint16_t m_temp;
  uint32_t m_clockCount;

  // Lookup table
  std::vector<Instruction> m_lookup;

  // Bus which the CPU is connected to
  Bus* m_bus;
};
