cmake_minimum_required(VERSION 3.8)
project(PurpleGB)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

include_directories(lib/imgui)
include_directories(include)

add_library(
    imgui
    STATIC
    lib/imgui/imgui.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/backends/imgui_impl_sdl2.cpp
    lib/imgui/backends/imgui_impl_sdlrenderer2.cpp
)


target_include_directories(imgui PUBLIC lib/imgui lib/imgui/backends  ${SDL2_INCLUDE_DIRS})
set_target_properties(imgui PROPERTIES FOLDER "imgui")

add_subdirectory(lib/ImGuiFileDialog)

file(GLOB_RECURSE EMU_SRC
    "src/*.cpp"
    "src/*.c"
    "include/*.hpp"
    "include/*.h"

    #imgui
    "lib/imgui/misc/cpp/*.cpp"
)

add_executable(purplegb ${EMU_SRC})
target_include_directories(purplegb PRIVATE include lib/imgui lib/imgui/backends lib/imgui/misc/cpp lib/ImGuiFileDialog)
target_link_libraries(purplegb PUBLIC SDL2::SDL2 SDL2_ttf::SDL2_ttf imgui ImGuiFileDialog)