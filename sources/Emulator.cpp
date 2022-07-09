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
      __debugbreak();
    }

    m_bus->ConnectCartridge(m_cartridge.get());
    m_ppu->ConnectCartridge(m_cartridge.get());

    // PowerUp
    m_cpu->PowerUp();
    m_ppu->Reset(); // TODO is ppu reset the same as power up?
  }
  /*
  {
    m_cartridge = std::make_unique<Cartridge>("");

    m_bus->ConnectCartridge(m_cartridge.get());
    m_ppu->ConnectCartridge(m_cartridge.get());

    // Set PC for cartridge
    m_cpu->SetProgramCounter(0x8000);
  }
  */
  // Extract dissassembly
  // TODO this needs some love
  //m_asm = m_cpu->Disassemble(0x0000, 0xFFFF);
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
  desc.Width = 128;
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

  // Palettes
  desc = {};
  desc.Width = 4 * 4 + 3;
  desc.Height = 1;
  desc.MipLevels = 1;
  desc.ArraySize = 1;
  desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.SampleDesc.Count = 1;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = 0;

  // Background palette
  subResource = {};
  subResource.pSysMem = m_ppu->GetPalleteColors(0).GetData();
  subResource.SysMemPitch = desc.Width * 4;
  subResource.SysMemSlicePitch = 0;
  d3dDevice->CreateTexture2D(&desc, &subResource, &m_texturePaletteBcg);

  srvDesc = {};
  srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = desc.MipLevels;
  srvDesc.Texture2D.MostDetailedMip = 0;
  d3dDevice->CreateShaderResourceView(m_texturePaletteBcg.Get(), &srvDesc, &m_paletteBcgView);

  // Foreground palette
  subResource = {};
  subResource.pSysMem = m_ppu->GetPalleteColors(1).GetData();
  subResource.SysMemPitch = desc.Width * 4;
  subResource.SysMemSlicePitch = 0;
  d3dDevice->CreateTexture2D(&desc, &subResource, &m_texturePaletteFrg);

  srvDesc = {};
  srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = desc.MipLevels;
  srvDesc.Texture2D.MostDetailedMip = 0;
  d3dDevice->CreateShaderResourceView(m_texturePaletteFrg.Get(), &srvDesc, &m_paletteFrgView);

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

  // Handle device buttons
  if (GetKey(tDX::Key::R).bPressed) Reset();

  // Handle hardware
  if (m_isRunning)
  {
    m_oscillator->Input(fElapsedTime);
  }
  else
  {
    if (GetKey(tDX::Key::I).bPressed)
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

  static const ImVec4 m_redColor(1.0f, 0.0f, 0.0f, 1.0f);
  static const ImVec4 m_greenColor(0.0f, 1.0f, 0.0f, 1.0f);
  static const ImVec4 m_darkGrayColor(0.6f, 0.6f, 0.6f, 1.0f);

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
  string formatX = hex(m_cpu->GetXRegister(), 2) + " [" + std::to_string(m_cpu->GetXRegister()) + "]";
  string formatY = hex(m_cpu->GetYRegister(), 2) + " [" + std::to_string(m_cpu->GetYRegister()) + "]";

  uint8_t status = m_cpu->GetStatusRegister();

  ImGui::Begin("CPU information");
  ImGui::Text("Bits");
  ImGui::Text("0 : Carry bit"); ImGui::SameLine(); print(status & CPU::Flags::C);
  ImGui::Text("1 : Zero"); ImGui::SameLine(); print(status & CPU::Flags::Z);
  ImGui::Text("2 : Disable Interrupts"); ImGui::SameLine(); print(status & CPU::Flags::I);
  ImGui::Text("3 : Decimal Mode"); ImGui::SameLine(); print(status & CPU::Flags::D);
  ImGui::Text("4 : Break"); ImGui::SameLine(); print(status & CPU::Flags::B);
  ImGui::Text("5 : UNUSED");
  ImGui::Text("6 : Overflow"); ImGui::SameLine(); print(status & CPU::Flags::V);
  ImGui::Text("7 : Negative"); ImGui::SameLine(); print(status & CPU::Flags::N);
  ImGui::Separator();
  ImGui::Columns(2);
  ImGui::SetColumnWidth(0, 120);
  ImGui::Text("Program counter"); ImGui::NextColumn(); ImGui::TextColored(m_darkGrayColor, hex(m_cpu->GetProgramCounter(), 4).data()); ImGui::NextColumn();
  ImGui::Text("Stack pointer"); ImGui::NextColumn(); ImGui::TextColored(m_darkGrayColor, hex(m_cpu->GetStakPointer(), 4).data()); ImGui::NextColumn();
  ImGui::Text("Accumulator"); ImGui::NextColumn(); ImGui::TextColored(m_darkGrayColor, formatAcc.data()); ImGui::NextColumn();
  ImGui::Text("Register X"); ImGui::NextColumn(); ImGui::TextColored(m_darkGrayColor, formatX.data()); ImGui::NextColumn();
  ImGui::Text("Register Y"); ImGui::NextColumn(); ImGui::TextColored(m_darkGrayColor, formatY.data()); ImGui::NextColumn();
  ImGui::End();

  /* TODO this needs to be done with respect to the places reachable by PC
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
  */

  ImGui::Begin("Memory window CPU range");
  ImGui::Text("Page: $"); ImGui::SameLine(); ImGui::InputText("", m_defaultPageCPU, 3, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);

  uint16_t addressPage;
  std::istringstream(m_defaultPageCPU) >> std::hex >> addressPage;

  addressPage = addressPage << 8;

  for (int row = 0; row < 16; row++)
  {
    stringstream memoryRow;
    memoryRow << "$" << uppercase << std::hex << m_defaultPageCPU << row << "0" << ":";

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

  tDX::Sprite& sprite1 = m_ppu->GetPatternTable(0, 0); // background tiles
  GetContext()->UpdateSubresource(m_textureP1.Get(), 0, NULL, sprite1.GetData(), sprite1.width * 4, 0);

  tDX::Sprite& sprite2 = m_ppu->GetPatternTable(1, 0); // foreground tiles (sprites)
  GetContext()->UpdateSubresource(m_textureP2.Get(), 0, NULL, sprite2.GetData(), sprite2.width * 4, 0);

  tDX::Sprite& sprite3 = m_ppu->GetPalleteColors(0); // background palette
  GetContext()->UpdateSubresource(m_texturePaletteBcg.Get(), 0, NULL, sprite3.GetData(), sprite3.width * 4, 0);

  tDX::Sprite& sprite4 = m_ppu->GetPalleteColors(1); // foreground palette
  GetContext()->UpdateSubresource(m_texturePaletteFrg.Get(), 0, NULL, sprite4.GetData(), sprite4.width * 4, 0);

  ImGui::Begin("Pattern table");
  ImGui::Columns(2);
  ImGui::SetColumnWidth(0, 148);
  // Pattern tables are rendered slightly bigger (133, 133) instead of (128, 128) to visually fit the palettes
  ImGui::Image((void*)m_patternTable1View.Get(), ImVec2(133, 133)); ImGui::NextColumn(); ImGui::Image((void*)m_patternTable2View.Get(), ImVec2(133, 133)); ImGui::NextColumn();
  ImGui::Image((void*)m_paletteBcgView.Get(), ImVec2(7 * ((4 * 4) + 3), 7)); ImGui::NextColumn(); ImGui::Image((void*)m_paletteFrgView.Get(), ImVec2(7 * ((4 * 4) + 3), 7));
  ImGui::End();

  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  return true;
}

bool Emulator::OnUserDestroy()
{
  // Cleanup ImGui
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  return true;
}

void Emulator::PrepareDisassembledCode(uint8_t lines)
{
  m_disassembledCode.clear();

  uint16_t pc = m_cpu->GetProgramCounter();
  auto it = m_asm.find(pc);

  int32_t emptyLinesStart = (*it).first - lines;
  int32_t highOffset = 2 * lines;

  if (emptyLinesStart < 0)
  {
    for (int i = 0; i < -emptyLinesStart; i++)
      m_disassembledCode.push_back("- out of bounds -");

    highOffset += emptyLinesStart;
  }

  int8_t offset = (*it).first - max(0, (*it).first - lines);
  advance(it, -offset);

  if (it != m_asm.end())
  {
    m_disassembledCode.push_back((*it).second);

    for (int i = 0; i < highOffset; i++)
    {
      if (++it != m_asm.end())
        m_disassembledCode.push_back((*it).second);
    }
  }
}

void Emulator::Reset()
{
  m_cpu->Reset();
  m_ppu->Reset();
  m_oscillator->Reset();
}
