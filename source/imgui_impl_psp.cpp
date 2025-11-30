#include <malloc.h>
#include <math.h>

#include <pspctrl.h>
#include <pspgu.h>
#include <psprtc.h>

#include "imgui.h"
#include "imgui_impl_psp.h"

namespace {

struct _psp_vertex {
    float u, v;
    unsigned int color;
    float x, y, z;
};

constexpr int _psp_screen_width = 480;
constexpr int _psp_screen_height = 272;
constexpr int _psp_pad_deadzone = 20;
constexpr float _psp_pad_speed = 5.0f;

static void* _font_texture = nullptr;
static int _font_texture_width = 0;
static int _font_texture_height = 0;
static int _font_texture_pot_width = 0;
static int _font_texture_pot_height = 0;
static ImVec2 _mouse_position(240, 136); // Center

static int _next_pot(const int v)
{
    int _result = 1;
    while (_result < v && _result < 512) {
        _result <<= 1;
    }
    return _result;
}
}

IMGUI_IMPL_API bool ImGui_ImplPSP_Init()
{
    ImGuiIO& _io = ImGui::GetIO();

    _io.BackendRendererName = "imgui_impl_pspgu";
    _io.BackendPlatformName = "imgui_impl_psp";
    _io.BackendFlags |= ImGuiBackendFlags_HasGamepad;

    // Display size fixed on PSP
    _io.DisplaySize = ImVec2(_psp_screen_width, _psp_screen_height);
    _io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    // Build font atlas
    unsigned char* _pixels = nullptr;
    int _width = 0, _height = 0;
    _io.Fonts->GetTexDataAsRGBA32(&_pixels, &_width, &_height);
    _font_texture_width = _width;
    _font_texture_height = _height;
    _font_texture_pot_width = _next_pot(_width);
    _font_texture_pot_height = _next_pot(_height);
    const std::size_t _texture_size = _font_texture_pot_width * _font_texture_pot_height * 4;
    _font_texture = memalign(16, _texture_size);
    if (!_font_texture) {
        return false;
    }
    memset(_font_texture, 0, _texture_size);
    for (int _y_index = 0; _y_index < _height; ++_y_index) {
        memcpy(
            (unsigned char*)_font_texture + _y_index * _font_texture_pot_width * 4,
            _pixels + _y_index * _width * 4,
            _width * 4);
    }
    _io.Fonts->TexID = (ImTextureID)_font_texture;

    // Init input
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    return true;
}

IMGUI_IMPL_API void ImGui_ImplPSP_Shutdown()
{
    ImGuiIO& _io = ImGui::GetIO();
    _io.Fonts->TexID = nullptr;
    if (_font_texture) {
        free(_font_texture);
        _font_texture = nullptr;
    }
}

IMGUI_IMPL_API void ImGui_ImplPSP_NewFrame()
{
    ImGuiIO& _io = ImGui::GetIO();

    // Delta time
    u64 _tick;
    sceRtcGetCurrentTick(&_tick);
    static u64 _last_tick = 0;
    if (_last_tick == 0) {
        _last_tick = _tick;
    }
    const float _delta_time = (float)(_tick - _last_tick) / (float)sceRtcGetTickResolution();
    _last_tick = _tick;
    _io.DeltaTime = _delta_time > 0.0f ? _delta_time : 1.0f / 60.0f;

    SceCtrlData _controller_pad {};
    sceCtrlReadBufferPositive(&_controller_pad, 1);

    // Input
    float _dx = (int)_controller_pad.Lx - 128;
    float _dy = (int)_controller_pad.Ly - 128;
    if (std::abs(_dx) < _psp_pad_deadzone) {
        _dx = 0.f;
    }
    if (std::abs(_dy) < _psp_pad_deadzone) {
        _dy = 0.f;
    }
    _mouse_position.x += _dx * _psp_pad_speed * (1.0f / 128.0f);
    _mouse_position.y += _dy * _psp_pad_speed * (1.0f / 128.0f);
    if (_mouse_position.x < 0.0f) {
        _mouse_position.x = 0.0f;
    }
    if (_mouse_position.y < 0.0f) {
        _mouse_position.y = 0.0f;
    }
    if (_mouse_position.x > 479.0f) {
        _mouse_position.x = 479.0f;
    }
    if (_mouse_position.y > 271.0f) {
        _mouse_position.y = 271.0f;
    }
    _io.AddMousePosEvent(_mouse_position.x, _mouse_position.y);
    _io.AddMouseButtonEvent(0, _controller_pad.Buttons & PSP_CTRL_CROSS);
    _io.AddKeyEvent(ImGuiKey_GamepadDpadUp, _controller_pad.Buttons & PSP_CTRL_UP);
    _io.AddKeyEvent(ImGuiKey_GamepadDpadDown, _controller_pad.Buttons & PSP_CTRL_DOWN);
    _io.AddKeyEvent(ImGuiKey_GamepadDpadLeft, _controller_pad.Buttons & PSP_CTRL_LEFT);
    _io.AddKeyEvent(ImGuiKey_GamepadDpadRight, _controller_pad.Buttons & PSP_CTRL_RIGHT);
    _io.AddKeyEvent(ImGuiKey_GamepadFaceDown, _controller_pad.Buttons & PSP_CTRL_CROSS);
    _io.AddKeyEvent(ImGuiKey_GamepadFaceRight, _controller_pad.Buttons & PSP_CTRL_CIRCLE);
    _io.AddKeyEvent(ImGuiKey_GamepadFaceLeft, _controller_pad.Buttons & PSP_CTRL_SQUARE);
    _io.AddKeyEvent(ImGuiKey_GamepadFaceUp, _controller_pad.Buttons & PSP_CTRL_TRIANGLE);
}

IMGUI_IMPL_API void ImGui_ImplPSP_RenderDrawData(ImDrawData* draw_data)
{
    if (!draw_data || draw_data->TotalVtxCount == 0) {
        return;
    }
    if (!_font_texture) {
        return;
    }

    // GU state
    sceGuDisable(GU_DEPTH_TEST);
    sceGuDepthMask(GU_TRUE);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuEnable(GU_BLEND);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuEnable(GU_TEXTURE_2D);
    sceGuTexMode(GU_PSM_8888, 0, 0, GU_FALSE);
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
    sceGuTexFilter(GU_LINEAR, GU_LINEAR);

    // No extra scaling/offset on texture
    sceGuTexScale(1.0f, 1.0f);
    sceGuTexOffset(0.0f, 0.0f);
    sceGuTexImage(
        0,
        _font_texture_pot_width,
        _font_texture_pot_height,
        _font_texture_pot_width,
        _font_texture);

    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList* _draw_list = draw_data->CmdLists[n];
        const ImDrawVert* _vertex_buffer = _draw_list->VtxBuffer.Data;
        const ImDrawIdx* _index_buffer = _draw_list->IdxBuffer.Data;
        int _index_offset = 0;

        for (int _command_index = 0; _command_index < _draw_list->CmdBuffer.Size; ++_command_index) {
            const ImDrawCmd* _draw_command = &_draw_list->CmdBuffer[_command_index];
            if (_draw_command->UserCallback) {
                _draw_command->UserCallback(_draw_list, _draw_command);
                _index_offset += _draw_command->ElemCount;
                continue;
            }

            // Clip rectangle directly in screen space
            ImVec4 _clip_rectangle = _draw_command->ClipRect;
            int _clip_x1 = (int)_clip_rectangle.x;
            int _clip_y1 = (int)_clip_rectangle.y;
            int _clip_x2 = (int)_clip_rectangle.z;
            int _clip_y2 = (int)_clip_rectangle.w;
            if (_clip_x1 < 0) {
                _clip_x1 = 0;
            }
            if (_clip_y1 < 0) {
                _clip_y1 = 0;
            }
            if (_clip_x2 > _psp_screen_width) {
                _clip_x2 = _psp_screen_width;
            }
            if (_clip_y2 > _psp_screen_height) {
                _clip_y2 = _psp_screen_height;
            }
            if (_clip_x2 <= _clip_x1 || _clip_y2 <= _clip_y1) {
                _index_offset += _draw_command->ElemCount;
                continue;
            }

            sceGuScissor(_clip_x1, _clip_y1, _clip_x2, _clip_y2);
            const int _index_count = _draw_command->ElemCount;
            _psp_vertex* _vertices = (_psp_vertex*)sceGuGetMemory(_index_count * sizeof(_psp_vertex));
            if (!_vertices) {
                return;
            }

            for (int _index = 0; _index < _index_count; ++_index) {
                ImDrawIdx idx = _index_buffer[_index_offset + _index];
                const ImDrawVert& _vertex = _vertex_buffer[idx];

                // Screen-space position
                _vertices[_index].x = (_vertex.pos.x - draw_data->DisplayPos.x) * draw_data->FramebufferScale.x;
                _vertices[_index].y = (_vertex.pos.y - draw_data->DisplayPos.y) * draw_data->FramebufferScale.y;
                _vertices[_index].z = 0.0f;

                // UV in texels
                _vertices[_index].u = _vertex.uv.x * (float)_font_texture_width;
                _vertices[_index].v = _vertex.uv.y * (float)_font_texture_height;

                _vertices[_index].color = _vertex.col;
            }

            // Not indexed triangles
            sceGuDrawArray(
                GU_TRIANGLES,
                GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D,
                _index_count,
                nullptr,
                _vertices);

            _index_offset += _draw_command->ElemCount;
        }
    }
}
