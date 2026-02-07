#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspkernel.h>

#include "imgui.h"
#include "backends/imgui_impl_psp.h"

PSP_MODULE_INFO("imgui_psp_demo", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#define BUFFER_WIDTH 512
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

// GU command list
static unsigned int __attribute__((aligned(16))) _gu_command_list[262144];

int _exit_callback(int arg1, int arg2, void* common)
{
    sceKernelExitGame();
    return 0;
}

int _callback_thread(SceSize args, void* argp)
{
    const int _cbid = sceKernelCreateCallback("exit_callback", _exit_callback, nullptr);
    sceKernelRegisterExitCallback(_cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int main()
{
    // Init debug
    pspDebugScreenInit();

    // Init callbacks
    const int _thread_id = sceKernelCreateThread("update_thread", _callback_thread, 0x11, 0xFA0, 0, 0);
    if (_thread_id >= 0) {
        sceKernelStartThread(_thread_id, 0, nullptr);
    }

    // Init GU
    sceGuInit();
    sceGuStart(GU_DIRECT, _gu_command_list);
    sceGuDrawBuffer(GU_PSM_8888, (void*)0, BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, (void*)(BUFFER_WIDTH * SCREEN_HEIGHT * 4), BUFFER_WIDTH);
    sceGuDepthBuffer((void*)(2 * BUFFER_WIDTH * SCREEN_HEIGHT * 4), BUFFER_WIDTH);
    sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuDepthMask(GU_TRUE);
    sceGuDisable(GU_DEPTH_TEST);
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);

    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.MouseDrawCursor = true;
    ImGui::StyleColorsDark();
    if (!ImGui_ImplPSP_Init()) {
        pspDebugScreenPrintf("Impossible to init PSP ImGui backend");
        sceKernelExitGame();
        return 0;
    }

    // Loop
    int running = 1;
    while (running) {

        ImGui_ImplPSP_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();

        sceGuStart(GU_DIRECT, _gu_command_list);
        sceGuClearColor(0xFF0000FF);
        sceGuClear(GU_COLOR_BUFFER_BIT);

        ImGui_ImplPSP_RenderDrawData(draw_data);

        sceGuFinish();
        sceGuSync(0, 0);
        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }

    sceGuTerm();
    sceKernelExitGame();
    return 0;
}
