cmake_minimum_required(VERSION 3.13)
project(imgui_psp)
set(imgui_version "1.92.5")

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

configure_file("imgui.pc.in" "${CMAKE_BINARY_DIR}/imgui.pc" @ONLY)
install(TARGETS "imgui" ARCHIVE DESTINATION "lib")
install(FILES "include/backends/imgui_impl_psp.h" DESTINATION "include/backends")
install(FILES "external/imgui/misc/cpp/imgui_stdlib.h"  DESTINATION "include/misc/cpp")
install(FILES 
		"external/imgui/imconfig.h" 
		"external/imgui/imgui_internal.h" 
		"external/imgui/imgui.h" 
		"external/imgui/imstb_rectpack.h" 
		"external/imgui/imstb_textedit.h" 
		"external/imgui/imstb_truetype.h" 		
	DESTINATION "include")
install(FILES "external/imgui/LICENSE.txt" DESTINATION "share/licenses/imgui")
install(FILES "${CMAKE_BINARY_DIR}/imgui.pc" DESTINATION "lib/pkgconfig")
install(DIRECTORY "external/imgui/docs/" DESTINATION "share/doc/imgui" FILES_MATCHING PATTERN "*.md")

