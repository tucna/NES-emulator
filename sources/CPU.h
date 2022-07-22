/*
  Addressing modes, lookup table and official instructions are taken from NES emulator of javidx9 - OneLoneCoder.com.
  Thank you for the great work!
*/

#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

/* CPU memory map
$0000 - $07FF $0800 2KB internal RAM
$0800 - $0FFF $0800 Mirrors of $0000 - $07FF
$1000 - $17FF $0800
$1800 - $1FFF $0800
$2000 - $2007 $0008 NES PPU registers
$2008 - $3FFF $1FF8 Mirrors of $2000 - 2007 (repeats every 8 bytes)
$4000 - $4017 $0018 NES APU and I / O registers
$4018 - $401F $0008 APU and I / O functionality that is normally disabled
Cartridge
$4020 - $5FFF $1FDF Cartridge Expansion ROM
$6000 - $7FFF $2000 SRAM
$8000 - $BFFF $4000 PRG-ROM
$C000 - $FFFF $4000 PRG-ROM
*/

class Bus;

// NES uses 2A03 CPU/APU based on 6502
class CPU
{
public:
  // Flags monitoring internal CPU state
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

  // Information related to the instructions
  struct Instruction
  {
    uint8_t opcode;
    std::string name; // only for debug
    uint8_t(CPU::*operate)(void);
    uint8_t(CPU::*addrmode)(void);
    uint8_t cycles;
  };

  // Constructor
  CPU();
  // Destructor
  ~CPU() {}

  // Signals
  void PowerUp();
  void Reset();
  void IRQ();
  void NMI();

  // Tick
  void Clock();

  // Connects CPU to a bus
  void ConnectToBus(Bus* bus) { m_bus = bus; }

  // Methods to make debugging easier
  std::map<uint16_t, std::string> Disassemble();

  bool IsInstructionCompleted() const { return m_cycles == 0; }

  uint8_t GetAcc() const { return m_a; }
  uint8_t GetXRegister() const { return m_x; }
  uint8_t GetYRegister() const { return m_y; }
  uint8_t GetStatusRegister() const { return m_status; }
  uint8_t GetStakPointer() const { return m_stackPointer; }

  uint16_t GetProgramCounter() const { return m_programCounter; }

  void SetProgramCounter(uint16_t programCounter) { m_programCounter = programCounter; }

private:
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

  // Unofficial opcodes
  uint8_t LAX();  uint8_t SAX();  uint8_t DCP();  uint8_t SRE();
  uint8_t RRA();  uint8_t RLA();  uint8_t ISC();  uint8_t SLO();
  uint8_t KIL();

  // TBD opcodes
  uint8_t XXX();

  // Generate write signal
  void Write(uint16_t addr, uint8_t data);
  // Generate read signal
  uint8_t Read(uint16_t addr);

  // Fetch data from the adress with respect to an addressing modes
  uint8_t Fetch();

  // Get CPU flag
  uint8_t GetFlag(Flags flag);
  // Set CPU flag
  void SetFlag(Flags flag, bool setClear);

  // Registers
  uint8_t m_a = 0;
  uint8_t m_x = 0;
  uint8_t m_y = 0;
  uint8_t m_stackPointer = 0;
  uint8_t m_status = 0; // sometimes called P

  uint16_t m_programCounter = 0;

  // Default program counter from reset vector
  uint16_t m_resetVectorPC = 0;
  // Input value
  uint8_t m_fetched = 0;
  // Instruction byte
  uint8_t m_opcode = 0;
  // How many cycles to finish current instruction
  uint8_t m_cycles = 0;
  // Memory address to be used
  uint16_t m_addrAbs = 0;
  // Relative adress to be used as increment/decrement
  uint16_t m_addrRel = 0;
  // Global number of clocks
  uint32_t m_clockCount = 0;
  // Nothing real, just to simplify things
  uint16_t m_temp = 0;

  // Lookup table
  std::vector<Instruction> m_lookup;

  // Bus to which the CPU is connected
  Bus* m_bus = nullptr;
};
