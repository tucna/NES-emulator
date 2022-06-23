#pragma once

#include <memory>

#include "engine/tPixelGameEngine.h"
#include "imgui/imgui.h"

#include "Bus.h"
#include "Cartridge.h"
#include "CPU.h"
#include "PPU.h"
#include "Oscillator.h"

class Emulator : public tDX::PixelGameEngine
{
public:
  Emulator();
  virtual ~Emulator();

  bool OnUserCreate() override;
  bool OnUserUpdate(float fElapsedTime) override;
  bool OnUserUpdateEndFrame(float fElapsedTime) override;
  bool OnUserDestroy() override;

private:
  void PrepareDisassembledCode(uint8_t lines);

  ImVec4 m_redColor       {1.0f, 0.0f, 0.0f, 1.0f};
  ImVec4 m_greenColor     {0.0f, 1.0f, 0.0f, 1.0f};
  ImVec4 m_darkGrayColor  {0.6f, 0.6f, 0.6f, 1.0f};

  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_patternTable1View;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_patternTable2View;

  Microsoft::WRL::ComPtr<ID3D11Texture2D> m_textureP1;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> m_textureP2;

  bool m_showUI = false;
  bool m_paused = true;
  bool m_oneStep = false;

  // Hardware
  std::array<uint8_t, 2 * 1024> m_ram; // TODO std::array<uint8_t, 2 * 1024> could be typedef
  std::unique_ptr<Bus> m_bus;
  std::unique_ptr<CPU> m_cpu;
  std::unique_ptr<PPU> m_ppu;
  std::unique_ptr<Oscillator> m_oscillator;

  // Peripherals
  std::unique_ptr<Cartridge> m_cartridge;

  // Disassembly
  std::map<uint16_t, std::string> m_asm;
  std::vector<std::string> m_disassembledCode;

};
