cmake_minimum_required (VERSION 3.1.0)
project (gb_emu)
set(COMPILE_FLAGS PUBLIC -ggdb3 -fPIC -Wall -Wextra -std=c++1y -DBOOST_LOG_DYN_LINK)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

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
  includes/graphics.hpp
  src/graphics.cpp
  includes/bootrom.hpp
  includes/memory.hpp
  includes/imemory.hpp
  src/memory.cpp)

target_include_directories(gb_lib PUBLIC includes)

target_compile_options(gb_lib ${COMPILE_FLAGS})


# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Core CONFIG REQUIRED)


add_executable(gb
    src/mainwindow.ui
    src/mainwindow.cpp
    includes/mainwindow.h
    src/main.cpp
  )

target_include_directories(gb PUBLIC includes)

target_link_libraries(gb
  gb_lib
  pthread
  boost_system boost_thread boost_log boost_log_setup
  Qt5::Widgets)

target_compile_options(gb ${COMPILE_FLAGS})

add_subdirectory(test)

