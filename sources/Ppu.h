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
class Ppu
{
public:
  Ppu();
  ~Ppu() {}

  void Clock();

  // TODO I do not like this logic
  void SetFrameIncomplete() { m_frameComplete = false; }

  bool IsFrameCompleted() const { return m_frameComplete; }

  void ConnectCartridge(Cartridge* cartridge);

  tDX::Sprite& GetScreen() { return m_sprScreen; }
  tDX::Sprite& GetPatternTable(uint8_t i, uint8_t palette);

  // Communication with Main Bus
  uint8_t ReadByCPU(uint16_t addr, bool rdonly = false);
  void WriteByCPU(uint16_t addr, uint8_t data);

  // Communication with PPU Bus
  uint8_t ReadByPPU(uint16_t addr, bool rdonly = false);
  void WriteByPPU(uint16_t addr, uint8_t data);

  bool nmi = false; // TUCNA

private:
  tDX::Pixel& GetColourFromPaletteRam(uint8_t palette, uint8_t pixel);

  union
  {
    struct
    {
      uint8_t grayscale : 1;
      uint8_t render_background_left : 1;
      uint8_t render_sprites_left : 1;
      uint8_t render_background : 1;
      uint8_t render_sprites : 1;
      uint8_t enhance_red : 1;
      uint8_t enhance_green : 1;
      uint8_t enhance_blue : 1;
    };

    uint8_t reg;
  } mask;

  union PPUCTRL
  {
    struct
    {
      uint8_t nametable_x : 1;
      uint8_t nametable_y : 1;
      uint8_t increment_mode : 1;
      uint8_t pattern_sprite : 1;
      uint8_t pattern_background : 1;
      uint8_t sprite_size : 1;
      uint8_t slave_mode : 1; // unused
      uint8_t enable_nmi : 1;
    };

    uint8_t reg;
  } control;

  union
  {
    struct
    {
      uint8_t unused : 5;
      uint8_t sprite_overflow : 1;
      uint8_t sprite_zero_hit : 1;
      uint8_t vertical_blank : 1;
    };

    uint8_t reg;
  } status;

  union loopy_register
  {
    // Credit to Loopy for working this out :D
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

  loopy_register vram_addr; // Active "pointer" address into nametable to extract background tile info
  loopy_register tram_addr; // Temporary store of information to be "transferred" into "pointer" at various times

  // Background rendering
  uint8_t bg_next_tile_id = 0x00;
  uint8_t bg_next_tile_attrib = 0x00;
  uint8_t bg_next_tile_lsb = 0x00;
  uint8_t bg_next_tile_msb = 0x00;
  uint16_t bg_shifter_pattern_lo = 0x0000;
  uint16_t bg_shifter_pattern_hi = 0x0000;
  uint16_t bg_shifter_attrib_lo = 0x0000;
  uint16_t bg_shifter_attrib_hi = 0x0000;

  // Pixel offset horizontally
  uint8_t fine_x = 0x00;

  // Internal communications
	uint8_t address_latch = 0x00; // TUCNA
	uint8_t ppu_data_buffer = 0x00;

  bool m_frameComplete;

  int16_t m_scanline;
  int16_t m_cycle;

  uint8_t m_tblName[2][1024]; // TUCNA
  uint8_t m_tblPalette[32];

  tDX::Pixel m_palScreen[0x40];
  tDX::Sprite m_sprScreen;
  tDX::Sprite m_sprPatternTable[2];

  Cartridge* m_cartridge;
};
