#define T_PGE_APPLICATION
#include "engine/tPixelGameEngine.h"

#include <iomanip>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "Bus.h"
#include "Cartridge.h"
#include "Common.h"
#include "CPU.h"
#include "Emulator.h"
#include "Oscillator.h"
#include "PPU.h"

using namespace std;

Emulator::Emulator()
{
  sAppName = "NES_Emulator";

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Create NES console HW
  m_bus = make_unique<Bus>();
  m_cpu = make_unique<CPU>();
  m_ppu = make_unique<PPU>();

  // Connect things over bus
  m_bus->ConnectCpu(m_cpu.get());
  m_bus->ConnectPPU(m_ppu.get());
  m_bus->ConnectRam(&m_ram);

  // Create oscillator and coonect with CPU and PPU
  m_oscillator = make_unique<Oscillator>(m_cpu.get(), m_ppu.get());

  // Create cartridge
  {
    m_cartridge = std::make_unique<Cartridge>("roms/cpu/nestest.nes");

    // Startup routine
    if (!m_cartridge->IsImageValid())
    {
      // TODO
    }

    m_bus->ConnectCartridge(m_cartridge.get());
    m_ppu->ConnectCartridge(m_cartridge.get());

    // Reset
    // TODO - more massive reset should happen?
    m_cpu->Reset();
    m_ppu->Reset();
  }
  /*
  {
    m_cartridge = std::make_unique<Cartridge>("");

    m_bus->ConnectCartridge(m_cartridge.get());
    m_ppu->ConnectCartridge(m_cartridge.get());

    // Set PC for cartridge
    m_cpu->SetProgramCounter(0x4020);
  }
  */
  // Extract dissassembly
  m_asm = m_cpu->Disassemble(0x0000, 0xFFFF);
}

Emulator::~Emulator()
{
}

bool Emulator::OnUserCreate()
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

  D3D11_SUBRESOURCE_DATA subResource = {};
  subResource.pSysMem = m_ppu->GetPatternTable(0, 0).GetData();
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

  subResource = {};
  subResource.pSysMem = m_ppu->GetPatternTable(1, 0).GetData();
  subResource.SysMemPitch = desc.Width * 4;
  subResource.SysMemSlicePitch = 0;
  d3dDevice->CreateTexture2D(&desc, &subResource, &m_textureP2);
  d3dDevice->CreateShaderResourceView(m_textureP2.Get(), &srvDesc, &m_patternTable2View);

  // Palette
  desc = {};
  desc.Width = 4;
  desc.Height = 1;
  desc.MipLevels = 1;
  desc.ArraySize = 1;
  desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.SampleDesc.Count = 1;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = 0;

  subResource = {};
  subResource.pSysMem = m_ppu->GetPalleteColors().GetData();
  subResource.SysMemPitch = desc.Width * 4;
  subResource.SysMemSlicePitch = 0;
  d3dDevice->CreateTexture2D(&desc, &subResource, &m_texturePalette);

  srvDesc = {};
  srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = desc.MipLevels;
  srvDesc.Texture2D.MostDetailedMip = 0;
  d3dDevice->CreateShaderResourceView(m_texturePalette.Get(), &srvDesc, &m_paletteView);

  return true;
}

bool Emulator::OnUserUpdate(float fElapsedTime)
{
  // Handle controller
  uint8_t& controller = m_bus->GetController1();

  controller = 0x00;
  controller |= GetKey(tDX::Key::X).bHeld ? 0x80 : 0x00;
  controller |= GetKey(tDX::Key::Z).bHeld ? 0x40 : 0x00;
  controller |= GetKey(tDX::Key::A).bHeld ? 0x20 : 0x00;
  controller |= GetKey(tDX::Key::S).bHeld ? 0x10 : 0x00;
  controller |= GetKey(tDX::Key::UP).bHeld ? 0x08 : 0x00;
  controller |= GetKey(tDX::Key::DOWN).bHeld ? 0x04 : 0x00;
  controller |= GetKey(tDX::Key::LEFT).bHeld ? 0x02 : 0x00;
  controller |= GetKey(tDX::Key::RIGHT).bHeld ? 0x01 : 0x00;

  // Handle debug
  if (GetKey(tDX::Key::F1).bPressed) m_showUI = !m_showUI;
  if (GetKey(tDX::Key::SPACE).bPressed) m_isRunning = !m_isRunning;

  // Handle hardware
  if (m_isRunning)
  {
    m_oscillator->Input(fElapsedTime);
  }
  else
  {
    if (GetKey(tDX::Key::C).bPressed)
      m_oscillator->TicksForCurrentInstruction();

    if (GetKey(tDX::Key::F).bPressed)
      m_oscillator->TicksForCurrentFrame();
  }

  // Handle output to television
  Clear(tDX::BLACK);
  DrawSprite(0, 0, &m_ppu->GetScreen(), 2);

  return true;
}

bool Emulator::OnUserUpdateEndFrame(float fElapsedTime)
{
  if (!m_showUI)
    return true;

  const ImVec4 m_redColor(1.0f, 0.0f, 0.0f, 1.0f);
  const ImVec4 m_greenColor(0.0f, 1.0f, 0.0f, 1.0f);
  const ImVec4 m_darkGrayColor(0.6f, 0.6f, 0.6f, 1.0f);

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

  // Start the Dear ImGui frame
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  string formatAcc = hex(m_cpu->GetAcc(), 2) + " [" + std::to_string(m_cpu->GetAcc()) + "]";
  string formatX = hex(m_cpu->GetRegX(), 2) + " [" + std::to_string(m_cpu->GetRegX()) + "]";
  string formatY = hex(m_cpu->GetRegY(), 2) + " [" + std::to_string(m_cpu->GetRegY()) + "]";

  ImGui::Begin("CPU information");
  ImGui::Text("Bits");
  ImGui::Text("0 : Carry bit"); ImGui::SameLine(); print(m_cpu->GetB0());
  ImGui::Text("1 : Zero"); ImGui::SameLine(); print(m_cpu->GetB1());
  ImGui::Text("2 : Disable Interrupts"); ImGui::SameLine(); print(m_cpu->GetB2());
  ImGui::Text("3 : Decimal Mode"); ImGui::SameLine(); print(m_cpu->GetB3());
  ImGui::Text("4 : Break"); ImGui::SameLine(); print(m_cpu->GetB4());
  ImGui::Text("5 : UNUSED");
  ImGui::Text("6 : Overflow"); ImGui::SameLine(); print(m_cpu->GetB6());
  ImGui::Text("7 : Negative"); ImGui::SameLine(); print(m_cpu->GetB7());
  ImGui::Separator();
  ImGui::Columns(2);
  ImGui::SetColumnWidth(0, 120);
  ImGui::Text("Program counter"); ImGui::NextColumn(); ImGui::TextColored(m_darkGrayColor, hex(m_cpu->GetProgramCounter(), 4).data()); ImGui::NextColumn();
  ImGui::Text("Stack pointer"); ImGui::NextColumn(); ImGui::TextColored(m_darkGrayColor, hex(m_cpu->GetStakPointer(), 4).data()); ImGui::NextColumn();
  ImGui::Text("Accumulator"); ImGui::NextColumn(); ImGui::TextColored(m_darkGrayColor, formatAcc.data()); ImGui::NextColumn();
  ImGui::Text("Register X"); ImGui::NextColumn(); ImGui::TextColored(m_darkGrayColor, formatX.data()); ImGui::NextColumn();
  ImGui::Text("Register Y"); ImGui::NextColumn(); ImGui::TextColored(m_darkGrayColor, formatY.data()); ImGui::NextColumn();
  ImGui::End();

  PrepareDisassembledCode(5);
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

  ImGui::Begin("Memory window 1");
  char defaultPage[] = "40";
  ImGui::Text("Page: $"); ImGui::SameLine(); ImGui::InputText("", defaultPage, 3, ImGuiInputTextFlags_CharsHexadecimal);

  uint16_t addressPage;
  std::istringstream(defaultPage) >> std::hex >> addressPage;

  addressPage = addressPage << 8;

  for (int row = 0; row < 16; row++)
  {
    stringstream memoryRow;
    memoryRow << "$" << uppercase << std::hex << defaultPage << row << "0" << ":";

    ImGui::Text(memoryRow.str().c_str());

    for (int column = 0; column < 16; column++)
    {
      uint16_t address = (addressPage + 16 * row) + column;
      uint8_t data = m_bus->Read(address, true);
      memoryRow.str(std::string());

      memoryRow << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << +data; // +data for easy number conversion

      ImGui::SameLine();

      if (data == 0)
        ImGui::TextColored(m_darkGrayColor, memoryRow.str().c_str());
      else
        ImGui::TextColored(m_greenColor, memoryRow.str().c_str());
    }
  }
  ImGui::End();

  tDX::Sprite& sprite1 = m_ppu->GetPatternTable(0, 0);
  GetContext()->UpdateSubresource(m_textureP1.Get(), 0, NULL, sprite1.GetData(), sprite1.width * 4, 0);

  tDX::Sprite& sprite2 = m_ppu->GetPatternTable(1, 0);
  GetContext()->UpdateSubresource(m_textureP2.Get(), 0, NULL, sprite2.GetData(), sprite2.width * 4, 0);

  ImGui::Begin("Pattern table");
  ImGui::Image((void*)m_patternTable1View.Get(), ImVec2(128, 128)); ImGui::SameLine(); ImGui::Image((void*)m_patternTable2View.Get(), ImVec2(128, 128));
  ImGui::Image((void*)m_paletteView.Get(), ImVec2(7 * 4, 7));
  ImGui::End();

  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  return true;
}

bool Emulator::OnUserDestroy()
{
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  return true;
}

void Emulator::PrepareDisassembledCode(uint8_t lines)
{
  m_disassembledCode.clear();

  auto it = m_asm.find(m_cpu->GetProgramCounter());

  advance(it, -lines);

  if (it != m_asm.end())
  {
    m_disassembledCode.push_back((*it).second);

    for (int i = 0; i < lines * 2; i++)
    {
      if (++it != m_asm.end())
        m_disassembledCode.push_back((*it).second);
    }
  }
}
