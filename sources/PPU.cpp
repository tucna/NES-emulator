#include "engine/tPixelGameEngine.h"

#include "Bus.h"
#include "Cartridge.h"
#include "Common.h"
#include "PPU.h"

PPU::PPU()
{
  m_palScreen[0x00] = tDX::Pixel(84, 84, 84);
  m_palScreen[0x01] = tDX::Pixel(0, 30, 116);
  m_palScreen[0x02] = tDX::Pixel(8, 16, 144);
  m_palScreen[0x03] = tDX::Pixel(48, 0, 136);
  m_palScreen[0x04] = tDX::Pixel(68, 0, 100);
  m_palScreen[0x05] = tDX::Pixel(92, 0, 48);
  m_palScreen[0x06] = tDX::Pixel(84, 4, 0);
  m_palScreen[0x07] = tDX::Pixel(60, 24, 0);
  m_palScreen[0x08] = tDX::Pixel(32, 42, 0);
  m_palScreen[0x09] = tDX::Pixel(8, 58, 0);
  m_palScreen[0x0A] = tDX::Pixel(0, 64, 0);
  m_palScreen[0x0B] = tDX::Pixel(0, 60, 0);
  m_palScreen[0x0C] = tDX::Pixel(0, 50, 60);
  m_palScreen[0x0D] = tDX::Pixel(0, 0, 0);
  m_palScreen[0x0E] = tDX::Pixel(0, 0, 0);
  m_palScreen[0x0F] = tDX::Pixel(0, 0, 0);

  m_palScreen[0x10] = tDX::Pixel(152, 150, 152);
  m_palScreen[0x11] = tDX::Pixel(8, 76, 196);
  m_palScreen[0x12] = tDX::Pixel(48, 50, 236);
  m_palScreen[0x13] = tDX::Pixel(92, 30, 228);
  m_palScreen[0x14] = tDX::Pixel(136, 20, 176);
  m_palScreen[0x15] = tDX::Pixel(160, 20, 100);
  m_palScreen[0x16] = tDX::Pixel(152, 34, 32);
  m_palScreen[0x17] = tDX::Pixel(120, 60, 0);
  m_palScreen[0x18] = tDX::Pixel(84, 90, 0);
  m_palScreen[0x19] = tDX::Pixel(40, 114, 0);
  m_palScreen[0x1A] = tDX::Pixel(8, 124, 0);
  m_palScreen[0x1B] = tDX::Pixel(0, 118, 40);
  m_palScreen[0x1C] = tDX::Pixel(0, 102, 120);
  m_palScreen[0x1D] = tDX::Pixel(0, 0, 0);
  m_palScreen[0x1E] = tDX::Pixel(0, 0, 0);
  m_palScreen[0x1F] = tDX::Pixel(0, 0, 0);

  m_palScreen[0x20] = tDX::Pixel(236, 238, 236);
  m_palScreen[0x21] = tDX::Pixel(76, 154, 236);
  m_palScreen[0x22] = tDX::Pixel(120, 124, 236);
  m_palScreen[0x23] = tDX::Pixel(176, 98, 236);
  m_palScreen[0x24] = tDX::Pixel(228, 84, 236);
  m_palScreen[0x25] = tDX::Pixel(236, 88, 180);
  m_palScreen[0x26] = tDX::Pixel(236, 106, 100);
  m_palScreen[0x27] = tDX::Pixel(212, 136, 32);
  m_palScreen[0x28] = tDX::Pixel(160, 170, 0);
  m_palScreen[0x29] = tDX::Pixel(116, 196, 0);
  m_palScreen[0x2A] = tDX::Pixel(76, 208, 32);
  m_palScreen[0x2B] = tDX::Pixel(56, 204, 108);
  m_palScreen[0x2C] = tDX::Pixel(56, 180, 204);
  m_palScreen[0x2D] = tDX::Pixel(60, 60, 60);
  m_palScreen[0x2E] = tDX::Pixel(0, 0, 0);
  m_palScreen[0x2F] = tDX::Pixel(0, 0, 0);

  m_palScreen[0x30] = tDX::Pixel(236, 238, 236);
  m_palScreen[0x31] = tDX::Pixel(168, 204, 236);
  m_palScreen[0x32] = tDX::Pixel(188, 188, 236);
  m_palScreen[0x33] = tDX::Pixel(212, 178, 236);
  m_palScreen[0x34] = tDX::Pixel(236, 174, 236);
  m_palScreen[0x35] = tDX::Pixel(236, 174, 212);
  m_palScreen[0x36] = tDX::Pixel(236, 180, 176);
  m_palScreen[0x37] = tDX::Pixel(228, 196, 144);
  m_palScreen[0x38] = tDX::Pixel(204, 210, 120);
  m_palScreen[0x39] = tDX::Pixel(180, 222, 120);
  m_palScreen[0x3A] = tDX::Pixel(168, 226, 144);
  m_palScreen[0x3B] = tDX::Pixel(152, 226, 180);
  m_palScreen[0x3C] = tDX::Pixel(160, 214, 228);
  m_palScreen[0x3D] = tDX::Pixel(160, 162, 160);
  m_palScreen[0x3E] = tDX::Pixel(0, 0, 0);
  m_palScreen[0x3F] = tDX::Pixel(0, 0, 0);
}

void PPU::Clock()
{
  // As we progress through m_scanlines and m_cycles, the PPU is effectively
  // a state machine going through the motions of fetching background
  // information and sprite information, compositing them into a pixel
  // to be output.

  // The lambda functions (functions inside functions) contain the various
  // actions to be performed depending upon the output of the state machine
  // for a given m_scanline/m_cycle combination

  // ==============================================================================
  // Increment the background tile "pointer" one tile/column horizontally
  auto IncrementScrollX = [&]()
  {
    // Note: pixel perfect scrolling horizontally is handled by the
    // data shifters. Here we are operating in the spatial domain of
    // tiles, 8x8 pixel blocks.

    // Ony if rendering is enabled
    if (mask.render_background || mask.render_sprites)
    {
      // A single name table is 32x30 tiles. As we increment horizontally
      // we may cross into a neighbouring nametable, or wrap around to
      // a neighbouring nametable
      if (m_vram_addr.coarse_x == 31)
      {
        // Leaving nametable so wrap address round
        m_vram_addr.coarse_x = 0;
        // Flip target nametable bit
        m_vram_addr.nametable_x = ~m_vram_addr.nametable_x;
      }
      else
      {
        // Staying in current nametable, so just increment
        m_vram_addr.coarse_x++;
      }
    }
  };

  // ==============================================================================
  // Increment the background tile "pointer" one m_scanline vertically
  auto IncrementScrollY = [&]()
  {
    // Incrementing vertically is more complicated. The visible nametable
    // is 32x30 tiles, but in memory there is enough room for 32x32 tiles.
    // The bottom two rows of tiles are in fact not tiles at all, they
    // contain the "attribute" information for the entire table. This is
    // information that describes which palettes are used for different
    // regions of the nametable.

    // In addition, the NES doesnt scroll vertically in chunks of 8 pixels
    // i.e. the height of a tile, it can perform fine scrolling by using
    // the fine_y component of the register. This means an increment in Y
    // first adjusts the fine offset, but may need to adjust the whole
    // row offset, since fine_y is a value 0 to 7, and a row is 8 pixels high

    // Ony if rendering is enabled
    if (mask.render_background || mask.render_sprites)
    {
      // If possible, just increment the fine y offset
      if (m_vram_addr.fine_y < 7)
      {
        m_vram_addr.fine_y++;
      }
      else
      {
        // If we have gone beyond the height of a row, we need to
        // increment the row, potentially wrapping into neighbouring
        // vertical nametables. Dont forget however, the bottom two rows
        // do not contain tile information. The coarse y offset is used
        // to identify which row of the nametable we want, and the fine
        // y offset is the specific "m_scanline"

        // Reset fine y offset
        m_vram_addr.fine_y = 0;

        // Check if we need to swap vertical nametable targets
        if (m_vram_addr.coarse_y == 29)
        {
          // We do, so reset coarse y offset
          m_vram_addr.coarse_y = 0;
          // And flip the target nametable bit
          m_vram_addr.nametable_y = ~m_vram_addr.nametable_y;
        }
        else if (m_vram_addr.coarse_y == 31)
        {
          // In case the pointer is in the attribute memory, we
          // just wrap around the current nametable
          m_vram_addr.coarse_y = 0;
        }
        else
        {
          // None of the above boundary/wrapping conditions apply
          // so just increment the coarse y offset
          m_vram_addr.coarse_y++;
        }
      }
    }
  };

  // ==============================================================================
  // Transfer the temporarily stored horizontal nametable access information
  // into the "pointer". Note that fine x scrolling is not part of the "pointer"
  // addressing mechanism
  auto TransferAddressX = [&]()
  {
    // Ony if rendering is enabled
    if (mask.render_background || mask.render_sprites)
    {
      m_vram_addr.nametable_x = m_tram_addr.nametable_x;
      m_vram_addr.coarse_x = m_tram_addr.coarse_x;
    }
  };

  // ==============================================================================
  // Transfer the temporarily stored vertical nametable access information
  // into the "pointer". Note that fine y scrolling is part of the "pointer"
  // addressing mechanism
  auto TransferAddressY = [&]()
  {
    // Ony if rendering is enabled
    if (mask.render_background || mask.render_sprites)
    {
      m_vram_addr.fine_y = m_tram_addr.fine_y;
      m_vram_addr.nametable_y = m_tram_addr.nametable_y;
      m_vram_addr.coarse_y = m_tram_addr.coarse_y;
    }
  };


  // ==============================================================================
  // Prime the "in-effect" background tile shifters ready for outputting next
  // 8 pixels in m_scanline.
  auto LoadBackgroundShifters = [&]()
  {
    // Each PPU update we calculate one pixel. These shifters shift 1 bit along
    // feeding the pixel compositor with the binary information it needs. Its
    // 16 bits wide, because the top 8 bits are the current 8 pixels being drawn
    // and the bottom 8 bits are the next 8 pixels to be drawn. Naturally this means
    // the required bit is always the MSB of the shifter. However, "fine x" scrolling
    // plays a part in this too, whcih is seen later, so in fact we can choose
    // any one of the top 8 bits.
    bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | bg_next_tile_lsb;
    bg_shifter_pattern_hi = (bg_shifter_pattern_hi & 0xFF00) | bg_next_tile_msb;

    // Attribute bits do not change per pixel, rather they change every 8 pixels
    // but are synchronised with the pattern shifters for convenience, so here
    // we take the bottom 2 bits of the attribute word which represent which
    // palette is being used for the current 8 pixels and the next 8 pixels, and
    // "inflate" them to 8 bit words.
    bg_shifter_attrib_lo = (bg_shifter_attrib_lo & 0xFF00) | ((bg_next_tile_attrib & 0b01) ? 0xFF : 0x00);
    bg_shifter_attrib_hi = (bg_shifter_attrib_hi & 0xFF00) | ((bg_next_tile_attrib & 0b10) ? 0xFF : 0x00);
  };


  // ==============================================================================
  // Every m_cycle the shifters storing pattern and attribute information shift
  // their contents by 1 bit. This is because every m_cycle, the output progresses
  // by 1 pixel. This means relatively, the state of the shifter is in sync
  // with the pixels being drawn for that 8 pixel section of the m_scanline.
  auto UpdateShifters = [&]()
  {
    if (mask.render_background)
    {
      // Shifting background tile pattern row
      bg_shifter_pattern_lo <<= 1;
      bg_shifter_pattern_hi <<= 1;

      // Shifting palette attributes by 1
      bg_shifter_attrib_lo <<= 1;
      bg_shifter_attrib_hi <<= 1;
    }
  };

  // All but 1 of the secanlines is visible to the user. The pre-render m_scanline
  // at -1, is used to configure the "shifters" for the first visible m_scanline, 0.
  if (m_scanline >= -1 && m_scanline < 240)
  {
    if (m_scanline == 0 && m_cycle == 0)
    {
      // "Odd Frame" m_cycle skip
      m_cycle = 1;
    }

    if (m_scanline == -1 && m_cycle == 1)
    {
      // Effectively start of new frame, so clear vertical blank flag
      status.vertical_blank = 0;
    }


    if ((m_cycle >= 2 && m_cycle < 258) || (m_cycle >= 321 && m_cycle < 338))
    {
      UpdateShifters();


      // In these m_cycles we are collecting and working with visible data
      // The "shifters" have been preloaded by the end of the previous
      // m_scanline with the data for the start of this m_scanline. Once we
      // leave the visible region, we go dormant until the shifters are
      // preloaded for the next m_scanline.

      // Fortunately, for background rendering, we go through a fairly
      // repeatable sequence of events, every 2 clock m_cycles.
      switch ((m_cycle - 1) % 8)
      {
      case 0:
        // Load the current background tile pattern and attributes into the "shifter"
        LoadBackgroundShifters();

        // Fetch the next background tile ID
        // "(m_vram_addr.reg & 0x0FFF)" : Mask to 12 bits that are relevant
        // "| 0x2000"                 : Offset into nametable space on PPU address bus
        bg_next_tile_id = ReadByPPU(0x2000 | (m_vram_addr.reg & 0x0FFF));

        // Explanation:
        // The bottom 12 bits of the loopy register provide an index into
        // the 4 nametables, regardless of nametable mirroring configuration.
        // nametable_y(1) nametable_x(1) coarse_y(5) coarse_x(5)
        //
        // Consider a single nametable is a 32x32 array, and we have four of them
        //   0                1
        // 0 +----------------+----------------+
        //   |                |                |
        //   |                |                |
        //   |    (32x32)     |    (32x32)     |
        //   |                |                |
        //   |                |                |
        // 1 +----------------+----------------+
        //   |                |                |
        //   |                |                |
        //   |    (32x32)     |    (32x32)     |
        //   |                |                |
        //   |                |                |
        //   +----------------+----------------+
        //
        // This means there are 4096 potential locations in this array, which
        // just so happens to be 2^12!
        break;
      case 2:
        // Fetch the next background tile attribute. OK, so this one is a bit
        // more involved :P

        // Recall that each nametable has two rows of cells that are not tile
        // information, instead they represent the attribute information that
        // indicates which palettes are applied to which area on the screen.
        // Importantly (and frustratingly) there is not a 1 to 1 correspondance
        // between background tile and palette. Two rows of tile data holds
        // 64 attributes. Therfore we can assume that the attributes affect
        // 8x8 zones on the screen for that nametable. Given a working resolution
        // of 256x240, we can further assume that each zone is 32x32 pixels
        // in screen space, or 4x4 tiles. Four system palettes are allocated
        // to background rendering, so a palette can be specified using just
        // 2 bits. The attribute byte therefore can specify 4 distinct palettes.
        // Therefore we can even further assume that a single palette is
        // applied to a 2x2 tile combination of the 4x4 tile zone. The very fact
        // that background tiles "share" a palette locally is the reason why
        // in some games you see distortion in the colours at screen edges.

        // As before when choosing the tile ID, we can use the bottom 12 bits of
        // the loopy register, but we need to make the implementation "coarser"
        // because instead of a specific tile, we want the attribute byte for a
        // group of 4x4 tiles, or in other words, we divide our 32x32 address
        // by 4 to give us an equivalent 8x8 address, and we offset this address
        // into the attribute section of the target nametable.

        // Reconstruct the 12 bit loopy address into an offset into the
        // attribute memory

        // "(m_vram_addr.coarse_x >> 2)"        : integer divide coarse x by 4,
        //                                      from 5 bits to 3 bits
        // "((m_vram_addr.coarse_y >> 2) << 3)" : integer divide coarse y by 4,
        //                                      from 5 bits to 3 bits,
        //                                      shift to make room for coarse x

        // Result so far: YX00 00yy yxxx

        // All attribute memory begins at 0x03C0 within a nametable, so OR with
        // result to select target nametable, and attribute byte offset. Finally
        // OR with 0x2000 to offset into nametable address space on PPU bus.
        bg_next_tile_attrib = ReadByPPU(0x23C0 | (m_vram_addr.nametable_y << 11)
          | (m_vram_addr.nametable_x << 10)
          | ((m_vram_addr.coarse_y >> 2) << 3)
          | (m_vram_addr.coarse_x >> 2));

        // Right we've read the correct attribute byte for a specified address,
        // but the byte itself is broken down further into the 2x2 tile groups
        // in the 4x4 attribute zone.

        // The attribute byte is assembled thus: BR(76) BL(54) TR(32) TL(10)
        //
        // +----+----+			    +----+----+
        // | TL | TR |			    | ID | ID |
        // +----+----+ where TL =   +----+----+
        // | BL | BR |			    | ID | ID |
        // +----+----+			    +----+----+
        //
        // Since we know we can access a tile directly from the 12 bit address, we
        // can analyse the bottom bits of the coarse coordinates to provide us with
        // the correct offset into the 8-bit word, to yield the 2 bits we are
        // actually interested in which specifies the palette for the 2x2 group of
        // tiles. We know if "coarse y % 4" < 2 we are in the top half else bottom half.
        // Likewise if "coarse x % 4" < 2 we are in the left half else right half.
        // Ultimately we want the bottom two bits of our attribute word to be the
        // palette selected. So shift as required...
        if (m_vram_addr.coarse_y & 0x02) bg_next_tile_attrib >>= 4;
        if (m_vram_addr.coarse_x & 0x02) bg_next_tile_attrib >>= 2;
        bg_next_tile_attrib &= 0x03;
        break;

        // Compared to the last two, the next two are the easy ones... :P

      case 4:
        // Fetch the next background tile LSB bit plane from the pattern memory
        // The Tile ID has been read from the nametable. We will use this id to
        // index into the pattern memory to find the correct sprite (assuming
        // the sprites lie on 8x8 pixel boundaries in that memory, which they do
        // even though 8x16 sprites exist, as background tiles are always 8x8).
        //
        // Since the sprites are effectively 1 bit deep, but 8 pixels wide, we
        // can represent a whole sprite row as a single byte, so offsetting
        // into the pattern memory is easy. In total there is 8KB so we need a
        // 13 bit address.

        // "(control.pattern_background << 12)"  : the pattern memory selector
        //                                         from control register, either 0K
        //                                         or 4K offset
        // "((uint16_t)bg_next_tile_id << 4)"    : the tile id multiplied by 16, as
        //                                         2 lots of 8 rows of 8 bit pixels
        // "(m_vram_addr.fine_y)"                  : Offset into which row based on
        //                                         vertical scroll offset
        // "+ 0"                                 : Mental clarity for plane offset
        // Note: No PPU address bus offset required as it starts at 0x0000
        bg_next_tile_lsb = ReadByPPU((control.pattern_background << 12)
          + ((uint16_t)bg_next_tile_id << 4)
          + (m_vram_addr.fine_y) + 0);

        break;
      case 6:
        // Fetch the next background tile MSB bit plane from the pattern memory
        // This is the same as above, but has a +8 offset to select the next bit plane
        bg_next_tile_msb = ReadByPPU((control.pattern_background << 12)
          + ((uint16_t)bg_next_tile_id << 4)
          + (m_vram_addr.fine_y) + 8);
        break;
      case 7:
        // Increment the background tile "pointer" to the next tile horizontally
        // in the nametable memory. Note this may cross nametable boundaries which
        // is a little complex, but essential to implement scrolling
        IncrementScrollX();
        break;
      }
    }

    // End of a visible m_scanline, so increment downwards...
    if (m_cycle == 256)
    {
      IncrementScrollY();
    }

    //...and reset the x position
    if (m_cycle == 257)
    {
      LoadBackgroundShifters();
      TransferAddressX();
    }

    // Superfluous reads of tile id at end of m_scanline
    if (m_cycle == 338 || m_cycle == 340)
    {
      bg_next_tile_id = ReadByPPU(0x2000 | (m_vram_addr.reg & 0x0FFF));
    }

    if (m_scanline == -1 && m_cycle >= 280 && m_cycle < 305)
    {
      // End of vertical blank period so reset the Y address ready for rendering
      TransferAddressY();
    }
  }

  if (m_scanline == 240)
  {
    // Post Render Scanline - Do Nothing!
  }

  if (m_scanline >= 241 && m_scanline < 261)
  {
    //std::string TUCNA("TUCNA " + std::to_string(m_scanline) + "\n");
    //OutputDebugString(TUCNA.c_str());

    if (m_scanline == 241 && m_cycle == 1)
    {
      // Effectively end of frame, so set vertical blank flag
      status.vertical_blank = 1;

      // If the control register tells us to emit a NMI when
      // entering vertical blanking period, do it! The CPU
      // will be informed that rendering is complete so it can
      // perform operations with the PPU knowing it wont
      // produce visible artefacts
      if (control.enable_nmi)
        m_nmi = true;
    }
  }



  // Composition - We now have background pixel information for this m_cycle
  // At this point we are only interested in background

  uint8_t bg_pixel = 0x00;   // The 2-bit pixel to be rendered
  uint8_t bg_palette = 0x00; // The 3-bit index of the palette the pixel indexes

  // We only render backgrounds if the PPU is enabled to do so. Note if
  // background rendering is disabled, the pixel and palette combine
  // to form 0x00. This will fall through the colour tables to yield
  // the current background colour in effect
  if (mask.render_background)
  {
    // Handle Pixel Selection by selecting the relevant bit
    // depending upon fine x scolling. This has the effect of
    // offsetting ALL background rendering by a set number
    // of pixels, permitting smooth scrolling
    uint16_t bit_mux = 0x8000 >> m_fineX;

    // Select Plane pixels by extracting from the shifter
    // at the required location.
    uint8_t p0_pixel = (bg_shifter_pattern_lo & bit_mux) > 0;
    uint8_t p1_pixel = (bg_shifter_pattern_hi & bit_mux) > 0;

    // Combine to form pixel index
    bg_pixel = (p1_pixel << 1) | p0_pixel;

    // Get palette
    uint8_t bg_pal0 = (bg_shifter_attrib_lo & bit_mux) > 0;
    uint8_t bg_pal1 = (bg_shifter_attrib_hi & bit_mux) > 0;
    bg_palette = (bg_pal1 << 1) | bg_pal0;
  }


  // Now we have a final pixel colour, and a palette for this m_cycle
  // of the current m_scanline. Let's at long last, draw that ^&%*er :P

  m_sprScreen.SetPixel(m_cycle - 1, m_scanline, GetColourFromPaletteRam(bg_palette, bg_pixel));

  // Fake some noise for now
  //sprScreen.SetPixel(m_cycle - 1, m_scanline, palScreen[(rand() % 2) ? 0x3F : 0x30]);

  // Advance renderer - it never stops, it's relentless
  m_cycle++;
  //m_frameComplete = false
  if (m_cycle >= 341)
  {
    m_cycle = 0;
    m_scanline++;
    if (m_scanline >= 261)
    {
      m_scanline = -1;
      m_frameComplete = true;
    }
  }
}

void PPU::Reset()
{
  m_fineX = 0x00;
  m_address_latch = 0x00;
  m_ppu_data_buffer = 0x00;
  m_scanline = 0;
  m_cycle = 0;
  bg_next_tile_id = 0x00;
  bg_next_tile_attrib = 0x00;
  bg_next_tile_lsb = 0x00;
  bg_next_tile_msb = 0x00;
  bg_shifter_pattern_lo = 0x0000;
  bg_shifter_pattern_hi = 0x0000;
  bg_shifter_attrib_lo = 0x0000;
  bg_shifter_attrib_hi = 0x0000;
  status.reg = 0x00;
  mask.reg = 0x00;
  control.reg = 0x00;
  m_vram_addr.reg = 0x0000;
  m_tram_addr.reg = 0x0000;
}

void PPU::ConnectCartridge(Cartridge * cartridge)
{
  m_cartridge = cartridge;
}

tDX::Sprite& PPU::GetPatternTable(uint8_t i, uint8_t palette)
{
  // This function draw the CHR ROM for a given pattern table into
  // an olc::Sprite, using a specified palette. Pattern tables consist
  // of 16x16 "tiles or characters". It is independent of the running
  // emulation and using it does not change the systems state, though
  // it gets all the data it needs from the live system. Consequently,
  // if the game has not yet established palettes or mapped to relevant
  // CHR ROM banks, the sprite may look empty. This approach permits a
  // "live" extraction of the pattern table exactly how the NES, and
  // ultimately the player would see it.

  // A tile consists of 8x8 pixels. On the NES, pixels are 2 bits, which
  // gives an index into 4 different colours of a specific palette. There
  // are 8 palettes to choose from. Colour "0" in each palette is effectively
  // considered transparent, as those locations in memory "mirror" the global
  // background colour being used. This mechanics of this are shown in
  // detail in ReadByPPU() & ppuWrite()

  // Characters on NES
  // ~~~~~~~~~~~~~~~~~
  // The NES stores characters using 2-bit pixels. These are not stored sequentially
  // but in singular bit planes. For example:
  //
  // 2-Bit Pixels       LSB Bit Plane     MSB Bit Plane
  // 0 0 0 0 0 0 0 0	  0 0 0 0 0 0 0 0   0 0 0 0 0 0 0 0
  // 0 1 1 0 0 1 1 0	  0 1 1 0 0 1 1 0   0 0 0 0 0 0 0 0
  // 0 1 2 0 0 2 1 0	  0 1 1 0 0 1 1 0   0 0 1 0 0 1 0 0
  // 0 0 0 0 0 0 0 0 =  0 0 0 0 0 0 0 0 + 0 0 0 0 0 0 0 0
  // 0 1 1 0 0 1 1 0	  0 1 1 0 0 1 1 0   0 0 0 0 0 0 0 0
  // 0 0 1 1 1 1 0 0	  0 0 1 1 1 1 0 0   0 0 0 0 0 0 0 0
  // 0 0 0 2 2 0 0 0	  0 0 0 1 1 0 0 0   0 0 0 1 1 0 0 0
  // 0 0 0 0 0 0 0 0	  0 0 0 0 0 0 0 0   0 0 0 0 0 0 0 0
  //
  // The planes are stored as 8 bytes of LSB, followed by 8 bytes of MSB

  // Loop through all 16x16 tiles
  for (uint16_t nTileY = 0; nTileY < 16; nTileY++)
  {
    for (uint16_t nTileX = 0; nTileX < 16; nTileX++)
    {
      // Convert the 2D tile coordinate into a 1D offset into the pattern
      // table memory.
      uint16_t nOffset = nTileY * 256 + nTileX * 16;

      // Now loop through 8 rows of 8 pixels
      for (uint16_t row = 0; row < 8; row++)
      {
        // For each row, we need to read both bit planes of the character
        // in order to extract the least significant and most significant
        // bits of the 2 bit pixel value. in the CHR ROM, each character
        // is stored as 64 bits of lsb, followed by 64 bits of msb. This
        // conveniently means that two corresponding rows are always 8
        // bytes apart in memory.
        uint8_t tile_lsb = ReadByPPU(i * 0x1000 + nOffset + row + 0x0000);
        uint8_t tile_msb = ReadByPPU(i * 0x1000 + nOffset + row + 0x0008);


        // Now we have a single row of the two bit planes for the character
        // we need to iterate through the 8-bit words, combining them to give
        // us the final pixel index
        for (uint16_t col = 0; col < 8; col++)
        {
          // We can get the index value by simply adding the bits together
          // but we're only interested in the lsb of the row words because...
          uint8_t pixel = (tile_lsb & 0x01) + (tile_msb & 0x01);

          // ...we will shift the row words 1 bit right for each column of
          // the character.
          tile_lsb >>= 1; tile_msb >>= 1;

          // Now we know the location and NES pixel value for a specific location
          // in the pattern table, we can translate that to a screen colour, and an
          // (x,y) location in the sprite
          m_sprPatternTable[i].SetPixel
          (
            nTileX * 8 + (7 - col), // Because we are using the lsb of the row word first
                        // we are effectively reading the row from right
                        // to left, so we need to draw the row "backwards"
            nTileY * 8 + row,
            GetColourFromPaletteRam(palette, pixel)
          );
        }
      }
    }
  }

  // Finally return the updated sprite representing the pattern table
  return m_sprPatternTable[i];
}

tDX::Sprite& PPU::GetPalleteColors(uint8_t part)
{
  uint8_t from = part == 0 ? 0 : 4;
  uint8_t to = part == 0 ? 4 : 8;

  uint8_t currentCell = 0;

  for (uint8_t palette = from; palette < to; palette++)
  {
    for (int index = 0; index < 4; index++)
    {
      m_palette.SetPixel(currentCell, 0, GetColourFromPaletteRam(palette, index));
      currentCell++;
    }

    m_palette.SetPixel(currentCell, 0, tDX::BLANK);

    currentCell++;
  }

  return m_palette;
}

uint8_t PPU::ReadByCPU(uint16_t addr, bool rdonly)
{
  uint8_t data = 0x00;

  if (rdonly)
  {
    // Reading from PPU registers can affect their contents
    // so this read only option is used for examining the
    // state of the PPU without changing its state. This is
    // really only used in debug mode.
    switch (addr)
    {
    case 0x0000: // Control
      data = control.reg;
      break;
    case 0x0001: // Mask
      data = mask.reg;
      break;
    case 0x0002: // Status
      data = status.reg;
      break;
    case 0x0003: // OAM Address
      break;
    case 0x0004: // OAM Data
      break;
    case 0x0005: // Scroll
      break;
    case 0x0006: // PPU Address
      break;
    case 0x0007: // PPU Data
      break;
    }
  }
  else
  {
    // These are the live PPU registers that repsond
    // to being read from in various ways. Note that not
    // all the registers are capable of being read from
    // so they just return 0x00
    switch (addr)
    {
      // Control - Not readable
    case 0x0000: break;

      // Mask - Not Readable
    case 0x0001: break;

      // Status
    case 0x0002:
      // Reading from the status register has the effect of resetting
      // different parts of the circuit. Only the top three bits
      // contain status information, however it is possible that
      // some "noise" gets picked up on the bottom 5 bits which
      // represent the last PPU bus transaction. Some games "may"
      // use this noise as valid data (even though they probably
      // shouldn't)
      data = (status.reg & 0xE0) | (m_ppu_data_buffer & 0x1F);

      // Clear the vertical blanking flag
      status.vertical_blank = 0;

      // Reset Loopy's Address latch flag
      m_address_latch = 0;
      break;

      // OAM Address
    case 0x0003: break;

      // OAM Data
    case 0x0004: break;

      // Scroll - Not Readable
    case 0x0005: break;

      // PPU Address - Not Readable
    case 0x0006: break;

      // PPU Data
    case 0x0007:
      // Reads from the NameTable ram get delayed one m_cycle,
      // so output buffer which contains the data from the
      // previous read request
      data = m_ppu_data_buffer;
      // then update the buffer for next time
      m_ppu_data_buffer = ReadByPPU(m_vram_addr.reg);
      // However, if the address was in the palette range, the
      // data is not delayed, so it returns immediately
      if (m_vram_addr.reg >= 0x3F00) data = m_ppu_data_buffer;
      // All reads from PPU data automatically increment the nametable
      // address depending upon the mode set in the control register.
      // If set to vertical mode, the increment is 32, so it skips
      // one whole nametable row; in horizontal mode it just increments
      // by 1, moving to the next column
      m_vram_addr.reg += (control.increment_mode ? 32 : 1);
      break;
    }
  }

  return data;
}
void PPU::WriteByCPU(uint16_t addr, uint8_t data)
{
  switch (addr)
  {
  case 0x0000: // Control
    control.reg = data;
    m_tram_addr.nametable_x = control.nametable_x;
    m_tram_addr.nametable_y = control.nametable_y;
    break;
  case 0x0001: // Mask
    mask.reg = data;
    break;
  case 0x0002: // Status
    break;
  case 0x0003: // OAM Address
    break;
  case 0x0004: // OAM Data
    break;
  case 0x0005: // Scroll
    if (m_address_latch == 0)
    {
      // First write to scroll register contains X offset in pixel space
      // which we split into coarse and fine x values
      m_fineX = data & 0x07;
      m_tram_addr.coarse_x = data >> 3;
      m_address_latch = 1;
    }
    else
    {
      // First write to scroll register contains Y offset in pixel space
      // which we split into coarse and fine Y values
      m_tram_addr.fine_y = data & 0x07;
      m_tram_addr.coarse_y = data >> 3;
      m_address_latch = 0;
    }
    break;
  case 0x0006: // PPU Address
    if (m_address_latch == 0)
    {
      // PPU address bus can be accessed by CPU via the ADDR and DATA
      // registers. The fisrt write to this register latches the high byte
      // of the address, the second is the low byte. Note the writes
      // are stored in the tram register...
      m_tram_addr.reg = (uint16_t)((data & 0x3F) << 8) | (m_tram_addr.reg & 0x00FF);
      m_address_latch = 1;
    }
    else
    {
      // ...when a whole address has been written, the internal vram address
      // buffer is updated. Writing to the PPU is unwise during rendering
      // as the PPU will maintam the vram address automatically whilst
      // rendering the m_scanline position.
      m_tram_addr.reg = (m_tram_addr.reg & 0xFF00) | data;
      m_vram_addr = m_tram_addr;
      m_address_latch = 0;
    }
    break;
  case 0x0007: // PPU Data
    WriteByPPU(m_vram_addr.reg, data);
    // All writes from PPU data automatically increment the nametable
    // address depending upon the mode set in the control register.
    // If set to vertical mode, the increment is 32, so it skips
    // one whole nametable row; in horizontal mode it just increments
    // by 1, moving to the next column
    m_vram_addr.reg += (control.increment_mode ? 32 : 1);
    break;
  }
}

uint8_t PPU::ReadByPPU(uint16_t addr, bool rdonly)
{
  uint8_t data = 0x00;
  addr &= 0x3FFF;

  if (addr >= 0x0000 && addr <= 0x1FFF)
  {
    m_cartridge->ReadByPPU(addr, data);
  }
  else if (addr >= 0x2000 && addr <= 0x3EFF)
  {
    addr &= 0x0FFF;

    if (m_cartridge->GetMirroring() == Cartridge::Mirroring::Vertical)
    {
      // Vertical
      if (addr >= 0x0000 && addr <= 0x03FF)
        data = m_tblName[0][addr & 0x03FF];
      if (addr >= 0x0400 && addr <= 0x07FF)
        data = m_tblName[1][addr & 0x03FF];
      if (addr >= 0x0800 && addr <= 0x0BFF)
        data = m_tblName[0][addr & 0x03FF];
      if (addr >= 0x0C00 && addr <= 0x0FFF)
        data = m_tblName[1][addr & 0x03FF];
    }
    else if (m_cartridge->GetMirroring() == Cartridge::Mirroring::Horizontal)
    {
      // Horizontal
      if (addr >= 0x0000 && addr <= 0x03FF)
        data = m_tblName[0][addr & 0x03FF];
      if (addr >= 0x0400 && addr <= 0x07FF)
        data = m_tblName[0][addr & 0x03FF];
      if (addr >= 0x0800 && addr <= 0x0BFF)
        data = m_tblName[1][addr & 0x03FF];
      if (addr >= 0x0C00 && addr <= 0x0FFF)
        data = m_tblName[1][addr & 0x03FF];
    }
  }
  else if (addr >= 0x3F00 && addr <= 0x3FFF)
  {
    addr &= 0x001F;
    if (addr == 0x0010) addr = 0x0000;
    if (addr == 0x0014) addr = 0x0004;
    if (addr == 0x0018) addr = 0x0008;
    if (addr == 0x001C) addr = 0x000C;
    data = m_tblPalette[addr] & (mask.grayscale ? 0x30 : 0x3F);
  }

  return data;
}

void PPU::WriteByPPU(uint16_t addr, uint8_t data)
{
  addr &= 0x3FFF;

  if (addr >= 0x0000 && addr <= 0x1FFF)
  {
    m_cartridge->WriteByPPU(addr, data);
    //m_tblPattern[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
  }
  else if (addr >= 0x2000 && addr <= 0x3EFF)
  {
    addr &= 0x0FFF;
    if (m_cartridge->GetMirroring() == Cartridge::Mirroring::Vertical)
    {
      // Vertical
      if (addr >= 0x0000 && addr <= 0x03FF)
        m_tblName[0][addr & 0x03FF] = data;
      if (addr >= 0x0400 && addr <= 0x07FF)
        m_tblName[1][addr & 0x03FF] = data;
      if (addr >= 0x0800 && addr <= 0x0BFF)
        m_tblName[0][addr & 0x03FF] = data;
      if (addr >= 0x0C00 && addr <= 0x0FFF)
        m_tblName[1][addr & 0x03FF] = data;
    }
    else if (m_cartridge->GetMirroring() == Cartridge::Mirroring::Horizontal)
    {
      // Horizontal
      if (addr >= 0x0000 && addr <= 0x03FF)
        m_tblName[0][addr & 0x03FF] = data;
      if (addr >= 0x0400 && addr <= 0x07FF)
        m_tblName[0][addr & 0x03FF] = data;
      if (addr >= 0x0800 && addr <= 0x0BFF)
        m_tblName[1][addr & 0x03FF] = data;
      if (addr >= 0x0C00 && addr <= 0x0FFF)
        m_tblName[1][addr & 0x03FF] = data;
    }
  }
  else if (addr >= 0x3F00 && addr <= 0x3FFF)
  {
    addr &= 0x001F;
    if (addr == 0x0010) addr = 0x0000;
    if (addr == 0x0014) addr = 0x0004;
    if (addr == 0x0018) addr = 0x0008;
    if (addr == 0x001C) addr = 0x000C;
    m_tblPalette[addr] = data;
  }
}

tDX::Pixel& PPU::GetColourFromPaletteRam(uint8_t palette, uint8_t pixel)
{
  // This is a convenience function that takes a specified palette and pixel
  // index and returns the appropriate screen colour.
  // "0x3F00"       - Offset into PPU addressable range where palettes are stored
  // "palette << 2" - Each palette is 4 bytes in size
  // "pixel"        - Each pixel index is either 0, 1, 2 or 3
  // "& 0x3F"       - Stops us reading beyond the bounds of the palScreen array
  return m_palScreen[ReadByPPU(0x3F00 + (palette << 2) + pixel) & 0x3F];

  // Note: We dont access tblPalette directly here, instead we know that ReadByPPU()
  // will map the address onto the seperate small RAM attached to the PPU bus.
}

