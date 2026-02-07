cmake_minimum_required(VERSION 3.13)
project(imgui_psp)

option(IMGUIPSP_BUILD_DEMO "Build a demo executable" OFF)

set(imgui_sources
	"external/imgui/imgui.cpp"
	"external/imgui/imgui_widgets.cpp"
	"external/imgui/imgui_tables.cpp"
	"external/imgui/imgui_draw.cpp"
	"external/imgui/imgui_demo.cpp"
	"external/imgui/misc/cpp/imgui_stdlib.cpp"
    "source/imgui_impl_psp.cpp")
add_library(imgui STATIC ${imgui_sources})
set_target_properties(imgui PROPERTIES CXX_STANDARD 17)
target_compile_definitions(imgui PRIVATE IMGUI_DISABLE_DEFAULT_SHELL_FUNCTIONS)
target_include_directories(imgui PUBLIC "external/imgui")
target_link_libraries(imgui PUBLIC pspctrl pspgu psprtc)
target_include_directories(imgui PUBLIC "include")

if(IMGUIPSP_BUILD_DEMO)
    add_subdirectory("demo")
endif()

# include(GNUInstallDirs)

# install(TARGETS imgui_psp
#     ARCHIVE DESTINATION "psp/lib"
#     LIBRARY DESTINATION "psp/lib")

# install(DIRECTORY "include"
#     DESTINATION "psp/include"
#     FILES_MATCHING PATTERN "*.hpp")

# install(DIRECTORY "external/imgui/upstream/imgui"
#     DESTINATION "psp/include"
#     FILES_MATCHING PATTERN "*.h")

# install(FILES "LICENSE" "external/imgui/upstream/imgui/LICENCE.txt"
#     DESTINATION "psp/share/licenses/imgui"
# )