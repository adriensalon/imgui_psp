cmake_minimum_required(VERSION 3.13)
project(imgui_psp)

option(IMGUIPSP_BUILD_DEMO "Build a demo executable" ON)

add_subdirectory("external/imgui")

add_library(imgui_psp STATIC "source/imgui_impl_psp.cpp")
add_library(imgui ALIAS imgui_psp)
set_property(TARGET imgui_psp PROPERTY CXX_STANDARD 17)
target_link_libraries(imgui_psp PUBLIC imgui_upstream)
target_link_libraries(imgui_psp PUBLIC pspctrl)
target_link_libraries(imgui_psp PUBLIC pspgu)
target_link_libraries(imgui_psp PUBLIC psprtc)
target_include_directories(imgui_psp PUBLIC "include")

if(IMGUIPSP_BUILD_DEMO)
    add_subdirectory("demo")
endif()

    