/*
  Based on NES emulator of javidx9 - OneLoneCoder.com.
  Thank you for the great work!
*/

#pragma once

#include <cstdint>

#include "engine/tPixelGameEngine.h"

/* PPU memory map
$0000 - $0FFF $1000 Pattern table 0
$1000 - $1FFF $1000 Pattern table 1
$2000 - $23FF $0400 Nametable 0
$2400 - $27FF $0400 Nametable 1
$2800 - $2BFF $0400 Nametable 2
$2C00 - $2FFF $0400 Nametable 3
$3000 - $3EFF $0F00 Mirrors of $2000 - $2EFF
$3F00 - $3F1F $0020 Palette RAM indexes
$3F20 - $3FFF $00E0 Mirrors of $3F00 - $3F1F
*/

class Cartridge;

// NES uses 2C02
class PPU
{
public:
  // Constructor
  PPU();
  // Destructor
  ~PPU() {}

  // Tick
  void Clock();

  // Signals
  bool NMI() { return m_nmi; }
  void Reset();

  // Set reguest to "handled"
  void NMIHandled() { m_nmi = false; }

  // Connects cartridge to PPU bus
  void ConnectCartridge(Cartridge* cartridge);

  // Read access from CPU
  uint8_t ReadByCPU(uint16_t addr, bool rdonly = false);
  // Write access from CPU
  void WriteByCPU(uint16_t addr, uint8_t data);

  // Read access from PPU (PPU bus)
  uint8_t ReadByPPU(uint16_t addr, bool rdonly = false);
  // Write access from CPU (PPU bus)
  void WriteByPPU(uint16_t addr, uint8_t data);

  // Debug
  bool IsFrameCompleted() const { return m_frameComplete; }

  void SetFrameNotCompleted() { m_frameComplete = false; }

  tDX::Sprite& GetScreen() { return m_sprScreen; }
  tDX::Sprite& GetPatternTable(uint8_t i, uint8_t palette);
  tDX::Sprite& GetPalleteColors(uint8_t part);

private:
  // Render palettes
  tDX::Pixel& GetColourFromPaletteRam(uint8_t palette, uint8_t pixel);

  union
  {
    struct
    {
      // Base nametable address (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
      uint8_t nametable_x : 1;
      // Base nametable address (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
      uint8_t nametable_y : 1;
      // VRAM address increment per CPU read / write of PPUDATA (0: add 1, going across; 1: add 32, going down)
      uint8_t increment_mode : 1;
      // Sprite pattern table address for 8x8 sprites (0: $0000; 1: $1000; ignored in 8x16 mode)
      uint8_t pattern_sprite : 1;
      // Background pattern table address(0: $0000; 1: $1000)
      uint8_t pattern_background : 1;
      // Sprite size(0: 8x8 pixels; 1: 8x16 pixels – see PPU OAM#Byte 1)
      uint8_t sprite_size : 1;
      // PPU master / slave select (0: read backdrop from EXT pins; 1: output color on EXT pins)
      uint8_t slave_mode : 1;
      // Generate an NMI at the start of the vertical blanking interval (0: off; 1: on)
      uint8_t enable_nmi : 1;
    };

    uint8_t reg = 0x00;
  } PPUCTRL;

  union
  {
    struct
    {
      // Greyscale (0: normal color, 1 : produce a greyscale display)
      uint8_t grayscale : 1;
      // Show background in leftmost 8 pixels of screen, 0 : Hide
      uint8_t render_background_left : 1;
      // Show sprites in leftmost 8 pixels of screen, 0 : Hide
      uint8_t render_sprites_left : 1;
      // Show background
      uint8_t render_background : 1;
      // Show sprites
      uint8_t render_sprites : 1;
      // Emphasize red(green on PAL / Dendy)
      uint8_t enhance_red : 1;
      // Emphasize green(red on PAL / Dendy)
      uint8_t enhance_green : 1;
      // Emphasize blue
      uint8_t enhance_blue : 1;
    };

    uint8_t reg = 0x00;
  } PPUMASK;

  union
  {
    struct
    {
      // PPU open bus. Returns stale PPU bus contents
      uint8_t unused : 5;
      // Sprite overflow. The intent was for this flag to be set whenever more than eight sprites appear on a scanline
      uint8_t sprite_overflow : 1;
      // Sprite 0 Hit.Set when a nonzero pixel of sprite 0 overlaps a nonzero background pixel
      uint8_t sprite_zero_hit : 1;
      // Vertical blank has started (0: not in vblank; 1: in vblank)
      uint8_t vertical_blank : 1;
    };

    uint8_t reg = 0x00;
  } PPUSTATUS;

  union LoopyRegister
  {
    // Credit to Loopy for working this out
    struct
    {

      uint16_t coarse_x : 5;
      uint16_t coarse_y : 5;
      uint16_t nametable_x : 1;
      uint16_t nametable_y : 1;
      uint16_t fine_y : 3;
      uint16_t unused : 1;
    };

    uint16_t reg = 0x0000;
  };

  // Active "pointer" address into nametable to extract background tile info
  LoopyRegister m_vramAddr;
  // Temporary store of information to be "transferred" into "pointer" at various times
  LoopyRegister m_tramAddr;

  // Pixel offset horizontally
  uint8_t m_fineX = 0;

  // Address to read/write a data
  uint8_t m_addressLatch = 0x00;
  // The data
  uint8_t m_data = 0x00;

  // Debug

  // Signal
  bool m_nmi = false;

  // Row of the screen
  int16_t m_scanline = 0;
  // Column of the screen
  int16_t m_cycle = 0;

  // Background rendering
  uint8_t m_nextTileId = 0x00;
  uint8_t m_nextTileAttrib = 0x00;
  uint8_t m_nextTileLsb = 0x00;
  uint8_t m_nextTileMsb = 0x00;

  uint16_t m_shifterPatternLo = 0x0000;
  uint16_t m_shifterPatternHi = 0x0000;
  uint16_t m_shifterAttribLo = 0x0000;
  uint16_t m_shifterAttribHi = 0x0000;

  uint8_t m_tblName[2][1024]; // 32 x 32 tiles (30 x 32 for tiles, 2 x 32 for atributes
  uint8_t m_tblPalette[32]; // 8 palettes per 4 colors

  // Debug
  bool m_frameComplete = false;

  tDX::Pixel m_palScreen[0x40];

  tDX::Sprite m_sprScreen{ 256, 240 };
  tDX::Sprite m_sprPatternTable[2]{ {128, 128}, {128, 128} };
  tDX::Sprite m_palette{ (8 * 4) + 7, 1 };

  // Cartridge connected to PPU bus
  Cartridge* m_cartridge;
};
