/*
  Addressing modes, lookup table and official instructions are taken from NES emulator of javidx9 - OneLoneCoder.com.
  Thank you for the great work!
*/

// TODO! PPU has some issue - drawing is crossing the borders

#pragma once

#include <cstdint>

#include "engine/tPixelGameEngine.h"

/* PPU memory map
$0000 - $0FFF	$1000	Pattern table 0
$1000 - $1FFF	$1000	Pattern table 1
$2000 - $23FF	$0400	Nametable 0
$2400 - $27FF	$0400	Nametable 1
$2800 - $2BFF	$0400	Nametable 2
$2C00 - $2FFF	$0400	Nametable 3
$3000 - $3EFF	$0F00	Mirrors of $2000 - $2EFF
$3F00 - $3F1F	$0020	Palette RAM indexes
$3F20 - $3FFF	$00E0	Mirrors of $3F00 - $3F1F
*/

class Cartridge;

// NES uses 2C02
// Class covers PPU and its bus
class PPU
{
public:
  PPU();
  ~PPU() {}

  void Clock();

  bool IsFrameCompleted() const { return m_frameComplete; }

  // Requests
  bool NMI() { return m_nmi; }

  // Set reguest to "handled"
  void NMIHandled() { m_nmi = false; }

  void ConnectCartridge(Cartridge* cartridge);

  tDX::Sprite& GetScreen() { return m_sprScreen; }
  tDX::Sprite& GetPatternTable(uint8_t i, uint8_t palette);
  tDX::Sprite& GetPalleteColors();

  // Communication with Main Bus
  uint8_t ReadByCPU(uint16_t addr, bool rdonly = false);
  void WriteByCPU(uint16_t addr, uint8_t data);

  // Communication with PPU Bus
  uint8_t ReadByPPU(uint16_t addr, bool rdonly = false);
  void WriteByPPU(uint16_t addr, uint8_t data);

private:
  tDX::Pixel& GetColourFromPaletteRam(uint8_t palette, uint8_t pixel);

  union
  {
    struct
    {
      uint8_t Nx : 1; // nametable select
      uint8_t Ny : 1; // nametable select
      uint8_t I : 1; // increment mode
      uint8_t S : 1; // sprite tile select
      uint8_t B : 1; // background tile select
      uint8_t H : 1; // sprite height
      uint8_t P : 1; // PPU master/slave
      uint8_t V : 1; // NMI enable
    };

    uint8_t reg;
  } PPUCTRL;

  union
  {
    struct
    {
      uint8_t Gr : 1; // greyscale
      uint8_t m : 1; // background left column enable
      uint8_t M : 1; // sprite left column enable
      uint8_t b : 1; // background enable
      uint8_t s : 1; // sprite enable
      uint8_t R : 1; // color emphasis R
      uint8_t G : 1; // color emphasis G
      uint8_t B : 1; // color emphasis B
    };

    uint8_t reg;
  } PPUMASK;

  union
  {
    struct
    {
      uint8_t unused : 5; // read resets write pair for $2005 / $2006
      uint8_t O : 1; // sprite overflow
      uint8_t S : 1; // sprite 0 hit
      uint8_t V : 1; // vblank
    };

    uint8_t reg;
  } PPUSTATUS;

  union LoopyRegister
  {
    struct
    {

      uint16_t coarseX : 5;
      uint16_t coarseY : 5;
      uint16_t nametableX : 1;
      uint16_t nametableY : 1;
      uint16_t fineY : 3;
      uint16_t unused : 1;
    };

    uint16_t reg;
  };

  struct
  {
    uint8_t nextTileId;
    uint8_t nextTileAttr;
    uint8_t nextTileLsb;
    uint8_t nextTileMsb;

    uint16_t shifterPatternLo;
    uint16_t shifterPatternHi;
    uint16_t shifterAttrLo;
    uint16_t shifterAttribHi;
  } m_backgroudAttributes;

  LoopyRegister m_vramAddr;
  LoopyRegister m_tranAddr;

  // Pixel offset horizontally
  uint8_t m_fineX;

  // Internal communications
	uint8_t m_addressLatch;
	uint8_t m_dataBuffer;

  bool m_frameComplete;
  bool m_nmi;

  int16_t m_scanline;
  int16_t m_cycle;

  uint8_t m_tblName[2][1024];
  uint8_t m_tblPalette[32];

  tDX::Pixel m_palScreen[0x40];
  tDX::Sprite m_sprScreen;
  tDX::Sprite m_sprPatternTable[2];

  // TODO debug
  tDX::Sprite m_palette;

  Cartridge* m_cartridge;
};
