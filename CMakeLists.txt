cmake_minimum_required(VERSION 4.4)
project(bgl LANGUAGES CXX)

option(BGL_ENABLE_TESTS "Build the test suite" ON)

add_subdirectory(third_party)
add_subdirectory(src)
add_subdirectory(apps)

if(BGL_ENABLE_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
