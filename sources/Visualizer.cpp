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
  return true;
}

bool Visualizer::OnUserUpdateEndFrame(float fElapsedTime)
{
  // Start the Dear ImGui frame
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  ImGui::ShowDemoWindow();

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
