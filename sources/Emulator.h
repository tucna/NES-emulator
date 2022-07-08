#pragma once

#include "engine/tPixelGameEngine.h"

#include <memory>

#include "Common.h"

class Bus;
class CPU;
class PPU;
class Oscillator;
class Cartridge;

class Emulator : public tDX::PixelGameEngine
{
public:
  // Constructor
  Emulator();
  // Destructor - virtual to cover smart pointers
  virtual ~Emulator();

  // tDX create
  bool OnUserCreate() override;
  // tDX update
  bool OnUserUpdate(float fElapsedTime) override;
  // tDX end frame - used for ImGui
  bool OnUserUpdateEndFrame(float fElapsedTime) override;
  // tDX destroy
  bool OnUserDestroy() override;

private:
  // Prepare list of dissasembled instructions centered aroud PC in a range [PC - lines, PC + lines]
  void PrepareDisassembledCode(uint8_t lines);

  // Hardware
  RAM2KB m_ram;
  std::unique_ptr<Bus> m_bus;
  std::unique_ptr<CPU> m_cpu;
  std::unique_ptr<PPU> m_ppu;
  std::unique_ptr<Oscillator> m_oscillator;

  // Peripherals
  std::unique_ptr<Cartridge> m_cartridge;

  // Debug UI variables
  bool m_showUI = false;
  bool m_isRunning = false;

  char m_defaultPageCPU[3] = "80";

  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_patternTable1View;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_patternTable2View;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_paletteBcgView;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_paletteFrgView;

  Microsoft::WRL::ComPtr<ID3D11Texture2D> m_textureP1;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> m_textureP2;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texturePaletteBcg;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texturePaletteFrg;

  // Disassembly
  std::map<uint16_t, std::string> m_asm;
  std::vector<std::string> m_disassembledCode;
};
