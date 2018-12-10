cmake_minimum_required (VERSION 2.6)
project (gb_emu)
set(COMPILE_FLAGS PUBLIC -ggdb3 -fPIC -Wall -Wextra -std=c++1y)

add_library(gb_lib
  includes/ifileio.hpp
  includes/fileio.hpp
  src/fileio.cpp
  includes/iromloader.hpp
  includes/romloader.hpp
  src/romloader.cpp
  src/cpu.cpp
  includes/cpu.hpp
  includes/iinstructionhandler.hpp
  includes/instructionhandler.hpp
  src/instructionhandler.cpp
  includes/iinstructions.hpp
  includes/instructions.hpp
  includes/iinterupthandler.hpp
  includes/interupthandler.hpp
  src/interupthandler.cpp
  includes/itimer.hpp
  includes/timer.hpp
  src/timer.cpp
  includes/bootrom.hpp
  includes/memory.hpp
  includes/imemory.hpp
  src/memory.cpp)

target_include_directories(gb_lib PUBLIC includes)

target_compile_options(gb_lib ${COMPILE_FLAGS})

add_executable(gb
  src/main.cpp
  )

target_link_libraries(gb gb_lib)
target_compile_options(gb ${COMPILE_FLAGS})

add_subdirectory(test)

