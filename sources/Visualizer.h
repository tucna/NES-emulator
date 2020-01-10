#pragma once

#include "engine/tPixelGameEngine.h"

#include "imgui/imgui.h"

class NES;

class Visualizer : public tDX::PixelGameEngine
{
public:
  Visualizer(NES* nes);
  ~Visualizer() {}

  bool OnUserCreate() override;
  bool OnUserUpdate(float fElapsedTime) override;
  bool OnUserUpdateEndFrame(float fElapsedTime) override;
  bool OnUserDestroy() override;

private:
  void PrepareDisassembledCode(uint8_t lines);

  NES* m_nes;

  ImVec4 m_redColor;
  ImVec4 m_greenColor;
  ImVec4 m_darkGrayColor;

  std::vector<std::string> m_disassembledCode;

  float m_residualTime;

  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_patternTable1View;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_patternTable2View;

  Microsoft::WRL::ComPtr<ID3D11Texture2D> m_textureP1;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> m_textureP2;

  bool m_showUI;
};
