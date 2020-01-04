#include "engine/tPixelGameEngine.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "NES.h"
#include "Visualizer.h"


Visualizer::Visualizer(NES* nes) :
  m_nes(nes)
{
  sAppName = "NES_Emulator";

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  //ImGuiIO& io = ImGui::GetIO();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  if (Construct(400, 300, 2, 2))
    Start();
}

bool Visualizer::OnUserCreate()
{
  // Setup Platform/Renderer bindings
  ImGui_ImplWin32_Init(GetHWND());
  ImGui_ImplDX11_Init(GetDevice(), GetContext());

  return true;
}

bool Visualizer::OnUserUpdate(float fElapsedTime)
{
  DrawString(6, 6, "TUCNA", tDX::WHITE);
  DrawString(5, 5, "TUCNA", tDX::RED);

  return true;
}

bool Visualizer::OnUserUpdateEndFrame(float fElapsedTime)
{
  // Start the Dear ImGui frame
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  //ImGui::ShowDemoWindow();

  /*
  C = (1 << 0),	// Carry Bit
    Z = (1 << 1),	// Zero
    I = (1 << 2),	// Disable Interrupts
    D = (1 << 3),	// Decimal Mode (unused in this implementation)
    B = (1 << 4),	// Break
    U = (1 << 5),	// Unused
    V = (1 << 6),	// Overflow
    N = (1 << 7),	// Negative
    */

  ImGui::Begin("CPU information");
  ImGui::Text("Carry bit"); ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 0, 0, 1), "No");
  ImGui::Text("Zero"); ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 0, 0, 1), "No");
  ImGui::Text("Disable Interrupts"); ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 0, 0, 1), "No");
  ImGui::Text("Decimal Mode"); ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 0, 0, 1), "No");
  ImGui::Text("Break"); ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 0, 0, 1), "No");
  ImGui::Text("UNUSED");
  ImGui::Text("Negative"); ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 0, 0, 1), "No");
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
