cmake_minimum_required(VERSION 3.0.0)
project(Chip8Emu)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(chip src/main.c)
target_link_libraries(chip ${CONAN_LIBS})