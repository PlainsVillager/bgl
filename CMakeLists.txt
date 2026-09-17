cmake_minimum_required(VERSION 4.4)
project(bgl LANGUAGES CXX)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)

# targets
#
## bgl
add_executable(bgl main.cpp
        include/sha1.hpp
        include/Utility.h
        include/Instance.h
        include/Launcher.h
        src/Launcher.cpp
        src/Utility.cpp
        src/Instance.cpp
        include/configuration/Player.hpp
        src/configuration/Player.cpp
        include/configuration/PlayerManager.hpp
        src/configuration/PlayerManager.cpp)

target_include_directories(bgl PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(bgl PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(bgl PRIVATE cpr::cpr)
target_link_options(bgl PRIVATE -mconsole)

## test
enable_testing()

add_subdirectory(test)

### test01
add_test(
  NAME test01
  COMMAND test/Debug/test01.exe
)
