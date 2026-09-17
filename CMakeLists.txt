cmake_minimum_required(VERSION 4.4)
project(bgl LANGUAGES CXX)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)

# targets
#
## bgl
add_executable(bgl main.cpp
        core/include/sha1.hpp
        core/include/Utility.h
        core/include/Instance.h
        core/include/Launcher.h
        core/src/Launcher.cpp
        core/src/Utility.cpp
        core/src/Instance.cpp
        core/include/configuration/Player.hpp
        core/src/configuration/Player.cpp
        core/include/configuration/PlayerManager.hpp
        core/src/configuration/PlayerManager.cpp)

target_include_directories(bgl PRIVATE ${PROJECT_SOURCE_DIR}/core/include)

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
