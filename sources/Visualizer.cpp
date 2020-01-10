#include "engine/tPixelGameEngine.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "NES.h"
#include "Visualizer.h"

using namespace std;

Visualizer::Visualizer(NES* nes) :
  m_nes(nes),
  m_redColor(1, 0, 0, 1),
  m_greenColor(0, 1, 0, 1),
  m_darkGrayColor(0.6f, 0.6f, 0.6f, 1),
  m_residualTime(0),
  m_showUI(false)
{
  sAppName = "NES_Emulator";

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  if (Construct(256, 240, 2, 2))
    Start();
}

bool Visualizer::OnUserCreate()
{
  // Setup Platform/Renderer bindings
  ImGui_ImplWin32_Init(GetHWND());
  ImGui_ImplDX11_Init(GetDevice(), GetContext());

  // Setup debug textures
  ID3D11Device* d3dDevice = GetDevice();

  D3D11_TEXTURE2D_DESC desc = {};
  desc.Width = 128; // TODO - fixed values, not cool
  desc.Height = 128;
  desc.MipLevels = 1;
  desc.ArraySize = 1;
  desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.SampleDesc.Count = 1;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = 0;

  // 1
  tDX::Sprite& sprite1 = m_nes->GetPpu().GetPatternTable(0, 0);
  sprite1.SetPixel(0, 0, tDX::WHITE);
  sprite1.SetPixel(0, 1, tDX::WHITE);
  sprite1.SetPixel(0, 2, tDX::WHITE);
  sprite1.SetPixel(1, 0, tDX::WHITE);
  sprite1.SetPixel(2, 0, tDX::WHITE);

  sprite1.SetPixel(127, 127, tDX::WHITE);
  sprite1.SetPixel(127, 126, tDX::WHITE);
  sprite1.SetPixel(127, 125, tDX::WHITE);
  sprite1.SetPixel(126, 127, tDX::WHITE);
  sprite1.SetPixel(125, 127, tDX::WHITE);

  //ID3D11Texture2D *pTexture = NULL;
  D3D11_SUBRESOURCE_DATA subResource = {};
  subResource.pSysMem = sprite1.GetData();
  subResource.SysMemPitch = desc.Width * 4;
  subResource.SysMemSlicePitch = 0;
  d3dDevice->CreateTexture2D(&desc, &subResource, &m_textureP1);

  // Create texture view
  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
  srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = desc.MipLevels;
  srvDesc.Texture2D.MostDetailedMip = 0;
  d3dDevice->CreateShaderResourceView(m_textureP1.Get(), &srvDesc, &m_patternTable1View);

  // 2
  tDX::Sprite& sprite2 = m_nes->GetPpu().GetPatternTable(1, 0);
  sprite2.SetPixel(0, 0, tDX::RED);
  sprite2.SetPixel(0, 1, tDX::RED);
  sprite2.SetPixel(0, 2, tDX::RED);
  sprite2.SetPixel(1, 0, tDX::RED);
  sprite2.SetPixel(2, 0, tDX::RED);

  sprite2.SetPixel(127, 127, tDX::RED);
  sprite2.SetPixel(127, 126, tDX::RED);
  sprite2.SetPixel(127, 125, tDX::RED);
  sprite2.SetPixel(126, 127, tDX::RED);
  sprite2.SetPixel(125, 127, tDX::RED);

  subResource = {};
  subResource.pSysMem = sprite2.GetData();
  subResource.SysMemPitch = desc.Width * 4;
  subResource.SysMemSlicePitch = 0;
  d3dDevice->CreateTexture2D(&desc, &subResource, &m_textureP2);
  d3dDevice->CreateShaderResourceView(m_textureP2.Get(), &srvDesc, &m_patternTable2View);

  return true;
}

bool Visualizer::OnUserUpdate(float fElapsedTime)
{
  Clear(tDX::BLACK);

  if (GetKey(tDX::Key::F1).bPressed) m_showUI = !m_showUI;

  DrawString(6, 6, "TUCNA", tDX::WHITE);
  DrawString(5, 5, "TUCNA", tDX::RED);

  Cpu& cpu = m_nes->GetCpu();
  Ppu& ppu = m_nes->GetPpu();

  if (m_residualTime > 0.0f)
  {
    m_residualTime -= fElapsedTime;
  }
  else
  {
    m_residualTime += (1.0f / 60.0f) - fElapsedTime;
    do { m_nes->Clock(); } while (!ppu.IsFrameCompleted());
    ppu.SetFrameIncomplete();
  }

  DrawSprite(0, 0, &ppu.GetScreen(), 1); // TODO: original version has "2" not "1" scale
  DrawRect(0, 0, ScreenWidth() - 1, ScreenHeight() - 1, tDX::WHITE);

  return true;
}

bool Visualizer::OnUserUpdateEndFrame(float fElapsedTime)
{
  if (!m_showUI)
    return true;

  auto print = [&](bool flag)
  {
    flag ? ImGui::TextColored(m_greenColor, "Yes") : ImGui::TextColored(m_redColor, "No");
  };

  auto hex = [](uint32_t n, uint8_t d)
  {
    std::string s(d + 1, '0');
    s[0] = '$';

    for (int i = d; i >= 1; i--, n >>= 4)
      s[i] = "0123456789ABCDEF"[n & 0xF];

    return s;
  };

  const Cpu& cpu = m_nes->GetCpu();

  // Start the Dear ImGui frame
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  string formatAcc = hex(cpu.GetAcc(), 2) + " [" + std::to_string(cpu.GetAcc()) + "]";
  string formatX = hex(cpu.GetRegX(), 2) + " [" + std::to_string(cpu.GetRegX()) + "]";
  string formatY = hex(cpu.GetRegY(), 2) + " [" + std::to_string(cpu.GetRegY()) + "]";

  ImGui::Begin("CPU information");
  ImGui::Text("Bits");
  ImGui::Text("0 : Carry bit"); ImGui::SameLine(); print(cpu.GetB0());
  ImGui::Text("1 : Zero"); ImGui::SameLine(); print(cpu.GetB1());
  ImGui::Text("2 : Disable Interrupts"); ImGui::SameLine(); print(cpu.GetB2());
  ImGui::Text("3 : Decimal Mode"); ImGui::SameLine(); print(cpu.GetB3());
  ImGui::Text("4 : Break"); ImGui::SameLine(); print(cpu.GetB4());
  ImGui::Text("5 : UNUSED");
  ImGui::Text("6 : Overflow"); ImGui::SameLine(); print(cpu.GetB6());
  ImGui::Text("7 : Negative"); ImGui::SameLine(); print(cpu.GetB7());
  ImGui::Separator();
  ImGui::Columns(2);
  ImGui::SetColumnWidth(0, 120);
  ImGui::Text("Program counter"); ImGui::NextColumn(); ImGui::TextColored(m_darkGrayColor, hex(cpu.GetProgramCounter(), 4).data()); ImGui::NextColumn();
  ImGui::Text("Stack pointer"); ImGui::NextColumn(); ImGui::TextColored(m_darkGrayColor, hex(cpu.GetStakPointer(), 4).data()); ImGui::NextColumn();
  ImGui::Text("Accumulator"); ImGui::NextColumn(); ImGui::TextColored(m_darkGrayColor, formatAcc.data()); ImGui::NextColumn();
  ImGui::Text("Register X"); ImGui::NextColumn(); ImGui::TextColored(m_darkGrayColor, formatX.data()); ImGui::NextColumn();
  ImGui::Text("Register Y"); ImGui::NextColumn(); ImGui::TextColored(m_darkGrayColor, formatY.data()); ImGui::NextColumn();
  ImGui::End();

  PrepareDisassembledCode(5); // TODO
  auto middleDis = m_disassembledCode.begin() + m_disassembledCode.size() / 2;

  ImGui::Begin("Disassembly");
  for (auto it = m_disassembledCode.begin(); it < middleDis; it++)
    ImGui::TextColored(m_darkGrayColor, (*it).data());
  ImGui::Separator();
  ImGui::Text((*middleDis++).data());
  ImGui::Separator();
  for (auto it = middleDis; it < m_disassembledCode.end(); it++)
    ImGui::TextColored(m_darkGrayColor, (*it).data());
  ImGui::End();

  tDX::Sprite& sprite1 = m_nes->GetPpu().GetPatternTable(0, 0);
  GetContext()->UpdateSubresource(m_textureP1.Get(), 0, NULL, sprite1.GetData(), sprite1.width * 4, 0);

  tDX::Sprite& sprite2 = m_nes->GetPpu().GetPatternTable(1, 0);
  GetContext()->UpdateSubresource(m_textureP2.Get(), 0, NULL, sprite2.GetData(), sprite2.width * 4, 0);

  ImGui::Begin("Pattern table");
  ImGui::Image((void*)m_patternTable1View.Get(), ImVec2(128, 128)); ImGui::SameLine(); ImGui::Image((void*)m_patternTable2View.Get(), ImVec2(128, 128));
  ImGui::End();

  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  return true;
}

bool Visualizer::OnUserDestroy()
{
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  return true;
}

void Visualizer::PrepareDisassembledCode(uint8_t lines)
{
  m_disassembledCode.clear();

  const Cpu& cpu = m_nes->GetCpu();
  const std::map<uint16_t, std::string>& assembly = m_nes->GetAssembly();

  auto it = assembly.find(cpu.GetProgramCounter());

  advance(it, -lines);

  if (it != assembly.end())
  {
    m_disassembledCode.push_back((*it).second);

    for (int i = 0; i < lines * 2; i++)
    {
      if (++it != assembly.end())
        m_disassembledCode.push_back((*it).second);
    }
  }
}
