cmake_minimum_required(VERSION 2.6)
# project(gb_emu)

################################
# GTest
################################

enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

################################
# Unit Tests
################################
# Add test cpp file
add_executable(gbTest
  romloader.t.cpp
  instructionhandler.t.cpp
  interupthandler.t.cpp
  cpu.t.cpp
  timer.t.cpp
  memory.t.cpp)
target_include_directories(gbTest PUBLIC ../includes)

target_compile_options(gbTest ${COMPILE_FLAGS})

# Link test executable against gtest & gtest_main
target_link_libraries(gbTest gb_lib gtest gtest_main gmock pthread)
add_test( gbTest gbTest )
