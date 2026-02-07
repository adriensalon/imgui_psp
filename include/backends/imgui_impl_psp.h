// dear imgui: Platform Backend for PSP
// This implements both the platform and the renderer PSP GU

// Implemented features:
//  [X] Platform: Mouse support from the pad.
//  [X] Platform: Gamepad support. Enabled with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.

#pragma once
#include "imgui.h"

IMGUI_IMPL_API bool ImGui_ImplPSP_Init();
IMGUI_IMPL_API void ImGui_ImplPSP_Shutdown();
IMGUI_IMPL_API void ImGui_ImplPSP_NewFrame();
IMGUI_IMPL_API void ImGui_ImplPSP_RenderDrawData(ImDrawData* draw_data);
