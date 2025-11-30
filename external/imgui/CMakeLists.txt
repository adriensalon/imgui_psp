set(imgui_sources
	"upstream/imgui/imgui.cpp"
	"upstream/imgui/imgui_widgets.cpp"
	"upstream/imgui/imgui_tables.cpp"
	"upstream/imgui/imgui_draw.cpp"
	"upstream/imgui/imgui_demo.cpp"
	"upstream/imgui/misc/cpp/imgui_stdlib.cpp")

add_library(imgui_upstream STATIC EXCLUDE_FROM_ALL ${imgui_sources})
set_target_properties(imgui_upstream PROPERTIES CXX_STANDARD 17)
target_include_directories(imgui_upstream PUBLIC "upstream/imgui")
    