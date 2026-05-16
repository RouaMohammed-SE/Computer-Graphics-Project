cmake_minimum_required(VERSION 3.20)
project(ComputerGraphics)

set(CMAKE_CXX_STANDARD 17)

# Collect all source files
file(GLOB_RECURSE SOURCES
        "*.cpp"
        "Algorithms/*.cpp"
        "App/*.cpp"
        "Clipping/*.cpp"
        "Core/*.cpp"
        "FileSystem/*.cpp"
        "Filling/*.cpp"
        "Menu/*.cpp"
        "Shapes/*.cpp"
        "Utils/*.cpp"
)

list(FILTER SOURCES EXCLUDE REGEX "cmake-build-debug/.*")

add_executable(ComputerGraphics WIN32 ${SOURCES}
        FillingCircles.cpp)

target_compile_definitions(ComputerGraphics PRIVATE UNICODE _UNICODE)

# Link WinAPI libraries needed by the project
target_link_libraries(ComputerGraphics comdlg32 gdi32)