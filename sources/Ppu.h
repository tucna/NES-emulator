#pragma once

#include <cstdint>

#include "Bus.h"

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

// NES uses 2C02
class Ppu
{
public:
  Ppu();
  ~Ppu() {}

  void Clock();

  // TODO I do not like this logic
  void SetFrameIncomplete() { m_frameComplete = false; }

  bool IsFrameCompleted() const { return m_frameComplete; }

  void ConnectToBus(Bus* bus) { m_bus = bus; }

private:
  bool m_frameComplete;

  int16_t m_scanline;
  int16_t m_cycle;

  Bus* m_bus;
};
