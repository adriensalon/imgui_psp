add_executable(imgui_psp_demo "main.cpp")
set_property(TARGET imgui_psp_demo PROPERTY CXX_STANDARD 17)
target_link_libraries(imgui_psp_demo PRIVATE imgui)
target_link_libraries(imgui_psp_demo PRIVATE pspdebug)
target_link_libraries(imgui_psp_demo PRIVATE pspge)
target_link_libraries(imgui_psp_demo PRIVATE pspdisplay)

create_pbp_file(
    TARGET imgui_psp_demo
    ICON_PATH NULL
    BACKGROUND_PATH NULL
    PREVIEW_PATH NULL
    TITLE imgui_psp_demo
    VERSION 01.00)