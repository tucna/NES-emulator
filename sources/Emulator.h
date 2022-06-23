#pragma once

#include <memory>

#include "engine/tPixelGameEngine.h"
#include "imgui/imgui.h"

#include "Bus.h"
#include "Cartridge.h"
#include "Cpu.h"
#include "Ppu.h"
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

  const std::map<uint16_t, std::string>& GetAssembly() { return m_asm; }

  // TODO
  uint8_t& GetContr() { return m_bus->GetController1(); }
  tDX::Sprite& TelevisionOutput() { return m_ppu->GetScreen(); }

  // Debug functions - nothing what NES really has
  Bus& GetBus() { return *m_bus; }
  Cpu& GetCpu() { return *m_cpu; }
  Ppu& GetPpu() { return *m_ppu; }
  std::array<uint8_t, 2 * 1024>& GetRam() { return m_ram; }

private:
  void PrepareDisassembledCode(uint8_t lines);
  void InsertCartridge(const std::string& file);
  void InsertDebugCartridge();

  ImVec4 m_redColor{1.0f, 0, 0, 1.0f};
  ImVec4 m_greenColor{0, 1.0f, 0, 1.0f};
  ImVec4 m_darkGrayColor{0.6f, 0.6f, 0.6f, 1.0f};

  std::vector<std::string> m_disassembledCode;

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
  std::unique_ptr<Cpu> m_cpu;
  std::unique_ptr<Ppu> m_ppu;
  std::unique_ptr<Oscillator> m_oscillator;
  std::unique_ptr<Cartridge> m_cartridge; // TODO Not so logical because NES does not "own" the cartridge, but it is simpler

  std::map<uint16_t, std::string> m_asm;
};
