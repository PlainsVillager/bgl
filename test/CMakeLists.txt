cmake_minimum_required(VERSION 4.4)

project(test01 LANGUAGES CXX)

add_executable(test01)
target_sources(test01 PRIVATE test01.cpp)
target_link_options(test01 PRIVATE -mconsole)
# target_link_libraries(test01 PRIVATE bgl)
