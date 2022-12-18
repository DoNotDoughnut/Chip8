cmake_minimum_required(VERSION 3.1)
project(Chip8Emu VERSION 0.1.0)

find_package(SDL2 REQUIRED)

add_executable(chip src/main.c)

target_link_libraries(chip PRIVATE ${SDL2_LIBRARIES})