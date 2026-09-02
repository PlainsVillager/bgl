cmake_minimum_required(VERSION 4.3)
project(bgl)

set(CMAKE_CXX_STANDARD 20)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)

add_executable(bgl main.cpp
        include/Utility.h
        include/LocalInstance.h
        include/Launcher.h
        src/Launcher.cpp
        src/Utility.cpp)

target_include_directories(bgl PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(bgl PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(bgl PRIVATE cpr::cpr)
