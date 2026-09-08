cmake_minimum_required(VERSION 4.4)
project(bgl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,bugprone-*,google-*,performance-*,modernize-*,clang-analyzer-core.*)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# vcpkg triplet: x64-mingw-static
find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)

add_executable(bgl main.cpp
        include/Utility.h
        include/Instance.h
        include/Launcher.h
        src/Launcher.cpp
        src/Utility.cpp
        src/Instance.cpp)

target_include_directories(bgl PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(bgl PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(bgl PRIVATE cpr::cpr)
